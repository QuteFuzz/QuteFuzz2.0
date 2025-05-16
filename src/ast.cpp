#include "../include/ast.h"

void Ast::resolve_dependency(std::shared_ptr<Node> initiator_node, Node_dependency& nd, Constraints::Constraints& constraints){
    auto n_completer_children = nd.get_num_completer_node_children();
    Term t = initiator_node->get_term();

    if((nd.get_initiator_hash() == Common::qreg_defs) && (nd.get_completer_hash() == Common::statements)){
        auto num_qreg_defs = Common::setup_qregs(qreg_defs, n_completer_children);

        constraints.add_constraint(Constraints::Constraint(Common::qreg_defs, Constraints::NUM_RULES_EQUALS, num_qreg_defs));

        initiator_node->save_branch(pick_branch(t.get_rule(), constraints).get_ok());
    }
}

/// @brief Transition from ready to done state iff:
/// - Chosen branch was empty 
/// - ALl children of this node have been built (are in NB_DONE state)
/// If node is done, set a flag in all dependencies where the node was an initiator
/// @param node 
Node_build_state Ast::node_ready_to_done(std::shared_ptr<Node> node, Branch& chosen_branch){

    if((chosen_branch.size() == 0) || node->children_built()){

        U64 node_hash = node->get_hash();

        for(Node_dependency& nd : node_deps){
            if(nd.node_is_initiator(node_hash)){
                nd.set_initiator_done();
            }
        }
        
        node->set_build_state(NB_DONE);
        return NB_DONE;
    }

    return NB_READY;
}


/// @brief Transition from stall to ready iff:
/// - Node isn't a completer of any dependencies
/// - Node is a completer of some dependecies, and all initiators in those dependencies are in NB_DONE
/// @param node 
Node_build_state Ast::node_stall_to_ready(std::shared_ptr<Node> node, Constraints::Constraints& constraints){
    U64 node_hash = node->get_hash();

    for(Node_dependency& nd : node_deps){
        /* 
            node in NB_STALL is an initiator and num completer children has been set
            set constraint on number of initiator pointer nodes based on number of completer pointer nodes, and use that to pick a new branch
            transition initiator from NB_STALL -> NB_READY
        */
        if(nd.node_is_initiator(node_hash)){
            if(nd.is_completer_children_set()){
                resolve_dependency(node, nd, constraints);

            } else {
                return NB_STALL;
            }
        }

        /*
            node in NB_STALL is a completer but initiator is !NB_DONE
            do not transition to ready, initiator must be done
            set number of pointer terms so that initiator can use this to transition from NB_STALL -> NB_READY -> NB_DONE

        */
        if(nd.node_is_completer(node_hash) && !nd.is_initiator_done()){
            nd.set_num_completer_node_children(node->get_branch().num_pointer_terms());
            return NB_STALL;
        }
    }

    node->set_build_state(NB_READY);

    return NB_READY;
}

/// @brief These are additional constraints on nodes which we cannot add directly in the grammar. 
/// This is why only "placeholder" nodes are considered in this switch statement
/// @param node 
/// @param constraints 
void Ast::add_constraint(std::shared_ptr<Node> node, Constraints::Constraints& constraints){
    auto hash = (Common::Rule_hash)node->get_hash(); 
    std::string str = node->get_string();

    switch(hash){
        
        case Common::program: case Common::circuit_def: case Common::qubit_list: case Common::parameter_list: case Common::parameter: case Common::statements: 
        case Common::qreg_decl: case Common::qreg_append: case Common::gate_application_kind: case Common::statement:
        case Common::InsertStrategy: case Common::arg_gate_application: case Common::phase_gate_application:
            // THESE ARE ACTUAL RULES IN THE GRAMMAR. Any constraints are already in the grammar definition
            break;

        case Common::imports:
            node->add_child(std::make_shared<Node>(imports()));
            break;

        case Common::compiler_call:
            node->add_child(std::make_shared<Node>(compiler_call()));
            break;

        case Common::qreg_name:
            node->add_child(std::make_shared<Node>(qreg_to_write->get_name()));
            break;

        case Common::qreg_size:
            node->add_child(std::make_shared<Node>(qreg_to_write->get_size_as_string()));
            break;   

        case Common::qubit_name: 
            node->add_child(std::make_shared<Node>(qubit_to_write->get_name()));
            break;
        
        case Common::qubit_index:
            node->add_child(std::make_shared<Node>(qubit_to_write->get_index_as_string()));
            break;   

        case Common::circuit_name:
            node->add_child(std::make_shared<Node>(Common::TOP_LEVEL_CIRCUIT_NAME));
            break;

        case Common::gate_name:  case Common::arg_gate_name: case Common::phase_gate_name:
            qreg_defs.reset_qubits();
            break;

        case Common::subroutines:
            break;

        case Common::qreg_defs:
            break;

        case Common::circuit:
            Common::setup_qregs(qreg_defs, 0);
            constraints.add_constraint(Constraints::Constraint(Common::qreg_defs, Constraints::NUM_RULES_EQUALS, qreg_defs.num_qregs()));
            break; 
            
        case Common::gate_application:
            constraints.clear();
            // constraints.add_constraint(Constraints::Constraint(Common::gate_name, Constraints::BRANCH_IN, {Common::h, Common::ccx, Common::cx}));
            break;

        case Common::qreg_def:
            qreg_to_write = qreg_defs.get_next_qreg();
            break;

        case Common::qubit:
            qubit_to_write = qreg_defs.get_random_qubit();
            break;

        case Common::float_literal:
            node->add_child(std::make_shared<Node>(std::to_string(random_float(0.5))));
            break;
        
        /*
            GATES. Make child for a syntax term that's just the name of the gate. Add constraint on the number of qubits that are chosen
        */
        case Common::h: case Common::x: case Common::y: case Common::z: case Common::s: case Common::t:
            node->add_child(std::make_shared<Node>(str));
            constraints.add_n_qubit_constrait(1);
            break;
        
        case Common::cx: case Common::cz: case Common::cnot:
            node->add_child(std::make_shared<Node>(str));
            constraints.add_n_qubit_constrait(2);
            break;

        case Common::ccx: case Common::cswap:
            node->add_child(std::make_shared<Node>(str));
            constraints.add_n_qubit_constrait(3);
            break;

        case Common::u1: case Common::rx: case Common::ry: case Common::rz: case Common::phasedxpowgate:
            node->add_child(std::make_shared<Node>(str));
            constraints.add_n_qubit_constrait(1, true);
            break;

        case Common::u2:
            node->add_child(std::make_shared<Node>(str));
            constraints.add_n_qubit_constrait(2, true);            
            break;

        case Common::u3: case Common::u:
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

    add_constraint(node, constraints);

    if(node->is_syntax() || (node->build_state() == NB_DONE)){
        node->set_build_state(NB_DONE);
        return;
    
    } else if (node->build_state() == NB_READY){
        Branch branch = node->get_branch();
        size_t num_children = node->get_num_children();

        for(size_t i = 0; i < branch.size(); i++){
            std::shared_ptr<Node> child_node;

            if(i < num_children){
                child_node = node->child_at(i);
            } else {
                child_node = std::make_shared<Node>(branch.at(i), node->get_depth() + 1);
                node->add_child(child_node); 
            }

            write_branch(child_node, constraints);
        }

        node_ready_to_done(node, branch);
        
    } else if (node->build_state() == NB_STALL){
        Term t = node->get_term();

        if(!node->has_chosen_branch()) {
            node->save_branch(pick_branch(t.get_rule(), constraints).get_ok());
        }

        // transition failed, move to next node
        if(node_stall_to_ready(node, constraints) == NB_STALL) return;

    }
    
    write_branch(node, constraints);
}

void Ast::ast_to_program(fs::path& path) {

    Result<Node, std::string> maybe_ast_root = build();

    if(maybe_ast_root.is_ok()){
        Node ast_root = maybe_ast_root.get_ok();
        std::ofstream stream(path.string());

        write(stream, ast_root);

        stream << "\"\"\" \n AST:\n" << std::endl;
        stream << ast_root << std::endl;
        stream << "\"\"\" " << std::endl;;

        std::cout << "Written to " << path.string() << std::endl;

    } else {
        ERROR(maybe_ast_root.get_error()); 
    }

};
