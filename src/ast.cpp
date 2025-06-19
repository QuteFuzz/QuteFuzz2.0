#include "../include/ast.h"


void Ast::transition_from_init(std::shared_ptr<Node> node){
    node->set_build_state(NB_READY);
}

void Ast::transition_from_ready(std::shared_ptr<Node> node){
	node->set_build_state(NB_DONE);
}

void Ast::set_best_qubit_pair(){
	std::vector<std::pair<int, int>> pairs = N_QUBIT_TO_PAIRINGS_MAP[get_current_qreg_defs()->num_qubits()];
	int score = -1000000;

	for(const auto& pair : pairs){
		// add entanglement to qig
		qig.add_edge(pair.first, pair.second);

		int curr_score = qig.score();

		if(curr_score > score){
			score = curr_score;
			best_qubit_pair = std::make_optional<std::pair<int, int>>(pair);
		}

		// make best pair if score is better than current best
		qig.remove_edge(pair.first, pair.second);
	}

	if(best_qubit_pair.has_value()){
		// commit best pair to qig
		qig.add_edge(best_qubit_pair.value().first, best_qubit_pair.value().second);
	}
}

void Ast::prepare_node(std::shared_ptr<Node> node){

	U64 hash = (Common::Rule_hash)node->get_hash(); 

	std::string str = node->get_string();

	switch(hash){
		
		case Common::program: case Common::circuit_def: case Common::qubit_list: case Common::parameter_list: 
		case Common::parameter: case Common::qreg_decl: case Common::qreg_append: 
		case Common::gate_application_kind: case Common::statement:
		case Common::InsertStrategy: case Common::arg_gate_application: case Common::phase_gate_application: 
			break;

		case Common::circuit: {
			if(in_subroutine()){
				/*
					currently building subroutine, track qreg defs dependency for each circuit
					store current state to restore later
				*/
				constraints.add_rules_constraint(Common::statements, Constraints::NUM_GIVEN_RULE_EQUALS, random_int(WILDCARD_MAX, 1), Common::statement);
				current_circuit_name = "sub"+std::to_string(current_subroutine++);

			} else {
				/*
					this is the main circuit
				*/
				constraints.add_rules_constraint(Common::statements, Constraints::NUM_GIVEN_RULE_EQUALS, random_int(WILDCARD_MAX, 20), Common::statement);
				current_circuit_name = Common::TOP_LEVEL_CIRCUIT_NAME;
				current_subroutine = 0;
			}

			break;
		}

		case Common::statements:
			constraints.allow_subroutines(can_apply_subroutines());
			break;
			
		case Common::subroutines:
			subs_node = node;
			constraints.add_rules_constraint(hash, Constraints::NUM_GIVEN_RULE_EQUALS, random_int(Common::MAX_SUBROUTINES), Common::circuit);
			break;

		case Common::gate_application:
			best_qubit_pair = std::nullopt;
			get_current_qreg_defs()->reset_qubits();
			break;

		case Common::subroutine: {				
			std::shared_ptr<Qreg_definitions> sub = get_random_subroutine();
			int num_sub_qubits = sub->num_qubits();

			if(num_sub_qubits == 2) {
				set_best_qubit_pair();
			}

			constraints.add_n_qubit_constraint(sub->num_qubits());
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
			node->add_child(std::make_shared<Node>(current_circuit_name));
			break;

		case Common::gate_name: case Common::arg_gate_name: case Common::phase_gate_name:
			break;

		case Common::qreg_defs: {
			std::shared_ptr<Qreg_definitions> current_defs = get_current_qreg_defs();

			constraints.add_rules_constraint(hash, Constraints::NUM_GIVEN_RULE_EQUALS, current_defs->setup_qregs((in_subroutine() ? 2 : get_max_defined_qubits())), Common::qreg_def);

			qig = Graph(current_defs->num_qubits());

			break;
		}

		case Common::qreg_def:
			qreg_to_write = get_current_qreg_defs()->get_next_qreg();
			break;

		case Common::qubit:
			qubit_to_write = get_current_qreg_defs()->get_random_qubit(best_qubit_pair);
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
			set_best_qubit_pair();
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

    Node_build_state old_build_state = node->build_state();

    if(node->is_syntax() || (old_build_state == NB_DONE)){
        node->set_build_state(NB_DONE);
		return;

	} else if (old_build_state == NB_READY){

        Branch branch = node->get_branch();

        for(size_t i = 0; i < branch.size(); i++){
            std::shared_ptr<Node> child_node;

			child_node = std::make_shared<Node>(branch.at(i), node->get_depth() + 1);
			node->add_child(child_node); 

            write_branch(child_node);
        }

        transition_from_ready(node);

    } else if (old_build_state == NB_INIT){
		prepare_node(node);

        Term t = node->get_term();
        
        if(!node->has_chosen_branch()) {
            node->save_branch(pick_branch(t.get_rule()).get_ok());
        }

        transition_from_init(node);
    
    } else {
        ERROR("Unknown build state!");
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
