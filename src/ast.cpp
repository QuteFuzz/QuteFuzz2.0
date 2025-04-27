#include "../include/ast.h"

/// @brief These are additional constraints on nodes which we cannot add directly in the grammar. 
/// This is why only "placeholder" nodes are considered in this switch statement
/// @param node 
/// @param constraints 
void Ast::add_constraint(std::shared_ptr<Node> node, Constraints::Constraints& constraints){
    auto hash = (Common::Rule_hash)node->get_hash(); 
    std::string str = node->get_string();

    switch(hash){
        
        case Common::program: case Common::circuit_def: case Common::qubit_list: case Common::parameter_list: case Common::parameter: case Common::statements: 
        case Common::qreg_defs: case Common::gate_application: case Common::gate_application_kind: case Common::statement:
            // THESE ARE ACTUAL RULES IN THE GRAMMAR. Any constraints are already in the grammar definition
            break;

        case Common::imports:
            node->add_child(std::make_shared<Node>(imports()));
            break;

        case Common::compiler_call:
            node->add_child(std::make_shared<Node>(compiler_call()));
            break;

        case Common::qreg_name: {
            node->add_child(std::make_shared<Node>(qreg_to_write.get_name()));
            break;
        }

        case Common::qreg_size: {
            node->add_child(std::make_shared<Node>(qreg_to_write.get_size_as_string()));
            break;   
        }

        case Common::lparen: case Common::rparen: case Common::comma: case Common::space: case Common::dot: 
        case Common::single_quote: case Common::double_pipe: case Common::double_quote: case Common::double_ampersand: case Common::equals:
            node->add_child(std::make_shared<Node>(Common::terminal_value(hash)));
            break;

        case Common::circuit_name:
            node->add_child(std::make_shared<Node>(Common::TOP_LEVEL_CIRCUIT_NAME));
            break;

        case Common::gate_name: 
            constraints.clear(); 
            qreg_defs.reset_qubits();
            break;

        case Common::circuit:
            Common::setup_qregs(qreg_defs);
            constraints.add_constraint(Constraints::Constraint(Common::qreg_defs, Constraints::NUM_RULES_EQUALS, qreg_defs.num_qregs()));
            break;            

        case Common::qreg_def:
            if(qreg_defs.defined()) qreg_to_write = qreg_defs.get_next_qreg();
            break;

        case Common::qubit:
            // choose random qubit
            node->add_child(std::make_shared<Node>(str)); //TODO
            break;

        case Common::float_literal:
            node->add_child(std::make_shared<Node>(std::to_string(random_float(0.5))));
            break;
        
        /*
            GATES. Make child for a syntax term that's just the name of the gate. Add constraint on the number of qubits that are chosen
        
        */
        case Common::h: case Common::x: case Common::y: case Common::z:
            node->add_child(std::make_shared<Node>(str));
            constraints.add_n_qubit_constrait(1);
            break;
        
        case Common::cx:
            node->add_child(std::make_shared<Node>(str));
            constraints.add_n_qubit_constrait(2);
            break;

        case Common::ccx:
            node->add_child(std::make_shared<Node>(str));
            constraints.add_n_qubit_constrait(3);
            break;

        case Common::u1: case Common::rx: case Common::ry: case Common::rz:
            node->add_child(std::make_shared<Node>(str));
            constraints.add_n_qubit_constrait(1, true);
            break;

        case Common::u2:
            node->add_child(std::make_shared<Node>(str));
            constraints.add_n_qubit_constrait(2, true);            
            break;

        case Common::u3:    
            node->add_child(std::make_shared<Node>(str));
            constraints.add_n_qubit_constrait(3, true);            
            break;   

        default:
            break;

    }
}

/// @brief Given a rule, pick one branch from that rule
/// @param rule 
/// @return 
Result<Branch, std::string> Ast::pick_branch(const std::shared_ptr<Rule> rule, Constraints::Constraints& constraints){
    Result<Branch, std::string> result;

    std::vector<Branch> branches = rule->get_branches();

    if(branches.empty()){
        result.set_ok(Branch());
        return result;
    }
    
    // if we have done a set number of recursions already and this rule has a non recursive branch, choose that instead
    if ((recursions <= 0) && rule->get_recursive_flag()){
        constraints.add_constraint(Constraints::Constraint(Constraints::BRANCH_IS_NON_RECURSIVE));
        result.set_ok(rule->pick_branch(constraints));   
        return result;
    }

    result.set_ok(rule->pick_branch(constraints));
    recursions -= 1;

    return result;
}

/// @brief Write branch terms to node children. Choosen branch must satisfy given constraints
/// @param node 
/// @param depth 
/// @param constraints 
void Ast::write_branch(std::shared_ptr<Node> node, Constraints::Constraints& constraints){
    Term t = node->get_term();

    if(t.is_pointer()){
        add_constraint(node, constraints);

        Result<Branch, std::string> maybe_branch = pick_branch(t.get_rule(), constraints);

        if(maybe_branch.is_ok()){
            Branch branch = maybe_branch.get_ok();
    
            for(const Term& t : branch.get_terms()){
                std::shared_ptr<Node> child = std::make_shared<Node>(t);
                
                write_branch(child, constraints);
    
                node->add_child(child);
            }
    
        } else {
            std::cout << maybe_branch.get_error() << std::endl;
        }

    }
    
}

void Ast::ast_to_program(fs::path& path) {

    Result<Node, std::string> maybe_ast_root = build();

    if(maybe_ast_root.is_ok()){
        Node ast_root = maybe_ast_root.get_ok();
        std::ofstream stream(path.string());

        write(stream, ast_root);

        std::cout << ast_root << std::endl;
        std::cout << "Written to " << path.string() << std::endl;

    } else {
        std::cout << "[ERROR] " << maybe_ast_root.get_error() << std::endl; 
    }

};
