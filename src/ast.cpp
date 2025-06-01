#include "../include/ast.h"

void Ast::resolve_dependency(std::shared_ptr<Node> initiator_node, int dependency_result){
    Term t = initiator_node->get_term();
    U64 node_hash = initiator_node->get_hash();
    size_t res = initiator_amount(node_hash, dependency_result).value();

    constraints.add_rules_constraint(node_hash, Constraints::NUM_RULES_EQUALS, res);
    initiator_node->save_branch(pick_branch(t.get_rule()).get_ok());

    node_deps.untrack_initiator();
}

/// @brief From init to ready, stall or init
/// @param node 
/// @param constraints 
/// @return 
Node_build_state Ast::transition_from_init(std::shared_ptr<Node> node){
    U64 node_hash = node->get_hash();

    if(node_deps.node_is(ND_INIT, node_hash) && !node_deps.no_outstanding_dependencies()){    
        if(node_deps.is_info_set()){
            resolve_dependency(node, node_deps.get_info());
        } else {
            return NB_INIT;
        }
    }

    if(node_deps.node_is(ND_COMP, node_hash) && !node_deps.no_outstanding_dependencies()){
        node_deps.increment_info();
        node->set_build_state(NB_STALL);
        return NB_STALL;
    }

    node->set_build_state(NB_READY);
    return NB_READY;
}

/// @brief From ready to done, stall or ready
/// @param node 
/// @param chosen_branch 
/// @return 
Node_build_state Ast::transition_from_ready(std::shared_ptr<Node> node, Branch& chosen_branch){

    if((chosen_branch.size() == 0) || node->children_built()){
        node->set_build_state(NB_DONE);
        return NB_DONE;
    
    } else if (node->get_children_in_state(NB_INIT) >= 1){
        return NB_READY;
    
    } else {        
        node->set_build_state(NB_STALL);
        return NB_STALL;
    }

}

/// @brief Completer nodes get stalled. This transitions to ready if dependencies have been resolved
/// Picks another branch in case constraints have changed 
/// @param node 
/// @return 
Node_build_state Ast::transition_from_stall(std::shared_ptr<Node> node){
    if(node_deps.no_outstanding_dependencies()){
	    Term t = node->get_term();

        node->set_build_state(NB_READY);
	    node->save_branch(pick_branch(t.get_rule()).get_ok());

        return NB_READY;
    } else {
        return NB_STALL;
    }
}

void Ast::prepare_node(std::shared_ptr<Node> node){

	U64 hash = (Common::Rule_hash)node->get_hash(); 

	if(node->build_state() == NB_INIT){

		std::string str = node->get_string();

		switch(hash){
			
			case Common::program: case Common::circuit_def: case Common::qubit_list: case Common::parameter_list: 
			case Common::parameter: case Common::qreg_decl: case Common::qreg_append: 
			case Common::gate_application_kind: case Common::statement:
			case Common::InsertStrategy: case Common::arg_gate_application: case Common::phase_gate_application: 
				break;

			case Common::circuit: {
				if(subs_node == nullptr){
					// subroutines rule not found 
				} else if (subs_node->build_state() == NB_READY){
					/*
						currently building subroutine, track qreg defs dependency for each circuit
						store current state to restore later
					*/
					node_deps = main_circ_deps.value_or(node_deps).get_subset(Common::qreg_defs);
					constraints.add_rules_constraint(Common::statements, Constraints::NUM_RULES_EQUALS, random_int(WILDCARD_MAX, 1));
					node->set_circuit_name("sub"+std::to_string(current_subroutine++));

				} else {
					/*
						this is the main circuit
					*/
					node->set_circuit_name(Common::TOP_LEVEL_CIRCUIT_NAME);
					current_subroutine = 0;
				}

				current_circuit_node = node;
				
				break;
			}

			case Common::qreg_defs:
				break;

			case Common::gate_application:
				break;

			case Common::statements:
				constraints.allow_subroutines(can_apply_subroutines());
				break;
				
			case Common::subroutines:
				subs_node = node;
				main_circ_deps = node_deps;
				break;

			case Common::subroutine: {
				auto sub = get_random_subroutine();
				constraints.add_n_qubit_constraint(sub->num_qubits());
				get_qreg_defs()->reset_qubits();

				node->add_child(std::make_shared<Node>(sub->owner()));
				break;
			}

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
				node->add_child(std::make_shared<Node>(current_circuit_node->get_circuit_name()));
				break;

			case Common::gate_name: case Common::arg_gate_name: case Common::phase_gate_name:
				get_qreg_defs()->reset_qubits();  // has to be called per gate name so that usability flags are reset before each gate application
				break;

			case Common::qreg_def:
				qreg_to_write = get_qreg_defs()->get_next_qreg();
				break;

			case Common::qubit:
				qubit_to_write = get_qreg_defs()->get_random_qubit();
				break;

			case Common::float_literal:
				node->add_child(std::make_shared<Node>(std::to_string(random_float(0.5))));
				break;
			
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

		initiator_default_setup(hash);

	} else if (hash == Common::statements) {
		constraints.allow_subroutines(can_apply_subroutines());

	} else if (hash == Common::circuit){
		current_circuit_node = node;
	}
}

/// @brief Given a rule, pick one branch from that rule
/// @param rule 
/// @return 
Result<Branch, std::string> Ast::pick_branch(const std::shared_ptr<Rule> rule){
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
void Ast::write_branch(std::shared_ptr<Node> node){

    Node_build_state new_build_state = NB_DONE, old_build_state = node->build_state();

	prepare_node(node);

    if(node->is_syntax() || (old_build_state == NB_DONE)){
        node->set_build_state(NB_DONE);
        new_build_state = NB_DONE;

    } else if (old_build_state == NB_STALL){
        new_build_state = transition_from_stall(node);

    } else if (old_build_state == NB_READY){
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

            write_branch(child_node);
        }

        new_build_state = transition_from_ready(node, branch);

    } else if (old_build_state == NB_INIT){
        Term t = node->get_term();
        
        if(!node->has_chosen_branch()) {
            node->save_branch(pick_branch(t.get_rule()).get_ok());
        }

        new_build_state = transition_from_init(node);
    
    } else {
        ERROR("Unknown build state!");
    }
    

    #if 0
	std::cout << constraints << std::endl;
    std::cout << *node << std::endl;
    getchar(); 
    #endif

    if((new_build_state != NB_READY) && (new_build_state == old_build_state)){
        return;
    }
    
    write_branch(node);
}

void Ast::ast_to_program(fs::path& path) {

    Result<Node, std::string> maybe_ast_root = build();

    if(maybe_ast_root.is_ok()){
        Node ast_root = maybe_ast_root.get_ok();
        std::ofstream stream(path.string());

        write(stream, ast_root);

        stream << "\"\"\" \n AST:\n" << std::endl;
        stream << ast_root << std::endl;
        stream << "\"\"\" " << std::endl;

        std::cout << "Written to " << path.string() << std::endl;

    } else {
        ERROR(maybe_ast_root.get_error()); 
    }

};
