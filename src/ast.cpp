#include "../include/ast.h"

void Ast::resolve_dependency(std::shared_ptr<Node> initiator_node, Node_dependency& nd, Constraints::Constraints& constraints){
    int info = nd.get_info();
    Term t = initiator_node->get_term();

    if((nd.get(ND_INIT) == Common::qreg_defs) && (nd.get(ND_COMP) == Common::gate_application)){
        auto num_qreg_defs = Common::setup_qregs(qreg_defs, info);

        constraints.add_rules_constraint(Common::qreg_defs, Constraints::NUM_RULES_EQUALS, num_qreg_defs);

        initiator_node->save_branch(pick_branch(t.get_rule(), constraints).get_ok());
    }
}

/// @brief From ready to done, stall or ready
/// @param node 
/// @param chosen_branch 
/// @return 
Node_build_state Ast::transition_from_ready(std::shared_ptr<Node> node, Branch& chosen_branch){

    // if all children are in done, this node is also in done,
    // if not, this done goes to stall state

    if((chosen_branch.size() == 0) || node->children_built()){

        // set flag for all dependencies where this node is an initiator, saying that it's done
        U64 node_hash = node->get_hash();

        if(!dependencies_in_flight.empty()){
            if(dependencies_in_flight.top()->node_is(ND_INIT, node_hash))
                dependencies_in_flight.top()->set_initiator_done();
        }

        node->set_build_state(NB_DONE);
        return NB_DONE;
    
    } else if (node->get_children_in_state(NB_INIT) >= 1){
        return NB_READY;
    
    } else {        
        node->set_build_state(NB_STALL);
        return NB_STALL;
    }

    // condition that allows node to stay in ready
}

/// @brief From init to ready, stall or init
/// @param node 
/// @param constraints 
/// @return 
Node_build_state Ast::transition_from_init(std::shared_ptr<Node> node, Constraints::Constraints& constraints){
    U64 node_hash = node->get_hash();

    for(Node_dependency& nd : node_deps){

        if(nd.node_is(ND_INIT, node_hash)){
            if(nd.is_info_set()){
                resolve_dependency(node, nd, constraints);

            } else {
                // add new dependency in flight on top of stack
                dependencies_in_flight.push(std::make_shared<Node_dependency>(nd));

                return NB_INIT;
            }
        }


        // if node is comp of any dependency, increment info of that dependency by 1, stay in stall state
        if(nd.node_is(ND_COMP, node_hash) && !nd.is_initiator_done()){
            nd.increment_info();
            node->set_build_state(NB_STALL);

            return NB_STALL;
        }
    }

    node->set_build_state(NB_READY);
    return NB_READY;
}

/// @brief These are additional constraints on nodes which we cannot add directly in the grammar. 
/// @param node 
/// @param constraints 
void Ast::add_constraint(std::shared_ptr<Node> node, Constraints::Constraints& constraints){
    auto hash = (Common::Rule_hash)node->get_hash(); 
    std::string str = node->get_string();

    switch(hash){
        
        case Common::program: case Common::circuit_def: case Common::qubit_list: case Common::parameter_list: case Common::parameter: case Common::statements: 
        case Common::qreg_decl: case Common::qreg_append: case Common::gate_application_kind: case Common::statement:
        case Common::qreg_defs: case Common::gate_application:
        case Common::InsertStrategy: case Common::arg_gate_application: case Common::phase_gate_application: case Common::circuit: 
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
            qreg_defs.reset_qubits();  // has to be called per gate name so that usability flags are reset before each gate application
            break;

        case Common::subroutines:
            // TODO?
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
            constraints.add_n_qubit_constraint(1);
            break;
        
        case Common::cx: case Common::cz: case Common::cnot:
            node->add_child(std::make_shared<Node>(str));
            constraints.add_n_qubit_constraint(2);
            break;

        case Common::ccx: case Common::cswap:
            node->add_child(std::make_shared<Node>(str));
            constraints.add_n_qubit_constraint(3);
            break;

        case Common::u1: case Common::rx: case Common::ry: case Common::rz: case Common::phasedxpowgate:
            node->add_child(std::make_shared<Node>(str));
            constraints.add_n_qubit_constraint(1, true);
            break;

        case Common::u2:
            node->add_child(std::make_shared<Node>(str));
            constraints.add_n_qubit_constraint(2, true);            
            break;

        case Common::u3: case Common::u:
            node->add_child(std::make_shared<Node>(str));
            constraints.add_n_qubit_constraint(3, true);            
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
        constraints.add_recursion_constraint();
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

    // std::cout << *node << std::endl;
    // getchar(); 

    if(node->is_syntax() || (node->build_state() == NB_DONE)){
        node->set_build_state(NB_DONE);
        return;

    } else if (node->build_state() == NB_STALL){
        
        if(dependencies_in_flight.top()->is_initiator_done()){
            node->set_build_state(NB_READY);
        } else {
            return;
        }

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

        transition_from_ready(node, branch);


    } else if (node->build_state() == NB_INIT){
        Term t = node->get_term();
        add_constraint(node, constraints);
        
        if(!node->has_chosen_branch()) {
            node->save_branch(pick_branch(t.get_rule(), constraints).get_ok());
        }

        if(transition_from_init(node, constraints) == NB_INIT) return;
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
