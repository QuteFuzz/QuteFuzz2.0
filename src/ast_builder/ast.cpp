#include "../../include/ast_builder/ast.h"


void Ast::transition_from_init(std::shared_ptr<Node> node){
    node->set_build_state(NB_READY);
}

void Ast::transition_from_ready(std::shared_ptr<Node> node){
	node->set_build_state(NB_DONE);
}

void Ast::prepare_node(std::shared_ptr<Node> node){

	U64 hash = (Common::Rule_hash)node->get_hash(); 

	std::string str = node->get_string();

	switch(hash){
		
		case Common::block: {
			context.maybe_render_qig();

			constraints.add_size_constraint(
				Common::body,
				Constraints::NUM_GIVEN_RULE_EQUALS,
				context.setup_block(),
				Common::statement
			);

			break;
		}

		case Common::body: {
			constraints.allow_subroutines(context.can_apply_subroutines());
			node -> set_indent_depth(node->get_indent_depth() + 1);
			context.set_qig();
			break;
		}

		case Common::circuit_id:
			node->add_child(std::make_shared<Node>(std::to_string(build_counter)));
			break;

		case Common::main_circuit_name:
			node->add_child(std::make_shared<Node>(Common::TOP_LEVEL_CIRCUIT_NAME));
			break;
			
		case Common::subroutines:
			constraints.constrain_num_subroutines();
			context.set_subroutines_node(node);
			break;

		case Common::gate_application:
			context.gate_application_reset();
			break;

		case Common::subroutine: {				
			std::shared_ptr<Block> subroutine = context.get_random_block();
			int num_sub_qubits = subroutine->num_qubits();

			context.set_best_entanglement(num_sub_qubits);
			node->add_child(std::make_shared<Node>(subroutine->owner()));
			constraints.add_n_qubit_constraint(num_sub_qubits);
			break;
		}
  
		case Common::circuit_name:
			context.set_circuit_name(node);
			break;

		case Common::qubit_defs : {
			constraints.add_size_constraint(
				hash, 
				Constraints::NUM_GIVEN_RULE_EQUALS, 
				context.setup_qubit_defs(),
				Common::qubit_def
			);

			break;
		}

		case Common::qubit_def : {
			constraints.constrain_qubit_def(context.set_qubit_def());			
			break;
		}

		case Common::qreg_name:
			context.set_qreg_name(node);
			break;

		case Common::qreg_size:
			context.set_qreg_size(node);
			break;  
			
		case Common::qubit: {
			bool from_register = context.set_qubit();
			constraints.constrain_qubit(from_register);
			break;
		}

		case Common::qubit_name:
			context.set_qubit_name(node);	
			break;
		
		case Common::qubit_index:
			context.set_qubit_index(node);
			break;

		case Common::float_literal:
			node->add_child(std::make_shared<Node>(std::to_string(random_float(0.5))));
			break;
		
		case Common::h: case Common::x: case Common::y: case Common::z: case Common::s: case Common::t:
		case Common::sdg: case Common::tdg: 
			node->add_child(std::make_shared<Node>(str));
			constraints.add_n_qubit_constraint(1);
			break;
		
		case Common::cx: case Common::cz: case Common::cnot: case Common::ch: case Common::crz: 
			node->add_child(std::make_shared<Node>(str));
			constraints.add_n_qubit_constraint(2);
			context.set_best_entanglement(2);
			break;

		case Common::ccx: case Common::cswap: case Common::toffoli:
			node->add_child(std::make_shared<Node>(str));
			constraints.add_n_qubit_constraint(3);
			context.set_best_entanglement(3);
			break;

		case Common::u1: case Common::rx: case Common::ry: case Common::rz:
			node->add_child(std::make_shared<Node>(str));
			constraints.add_n_qubit_constraint(1, true);
			constraints.add_size_constraint(Common::float_literals, Constraints::NUM_GIVEN_RULE_EQUALS, 1, Common::float_literal);
			break;

		case Common::u2:
			node->add_child(std::make_shared<Node>(str));
			constraints.add_n_qubit_constraint(1, true);
			constraints.add_size_constraint(Common::float_literals, Constraints::NUM_GIVEN_RULE_EQUALS, 2, Common::float_literal);
			break;

		case Common::u3: case Common::u:
			node->add_child(std::make_shared<Node>(str));	
			constraints.add_n_qubit_constraint(1, true);
			constraints.add_size_constraint(Common::float_literals, Constraints::NUM_GIVEN_RULE_EQUALS, 3, Common::float_literal);
			break;

		case Common::phasedxpowgate:
			node->add_child(std::make_shared<Node>(str));
			constraints.add_n_qubit_constraint(1, true);
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
			
			child_node = std::make_shared<Node>(branch.at(i), node->get_depth() + 1, node->get_indent_depth());
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

void Ast::ast_to_program(fs::path output_dir, const std::string& extension, int num_programs){

	for(build_counter = 0; build_counter < num_programs; build_counter++){
		fs::path current_circuit_dir =  output_dir / ("circuit" + std::to_string(build_counter));
		fs::create_directory(current_circuit_dir);

		context.set_circuits_dir(current_circuit_dir);
		
	    Result<Node, std::string> maybe_ast_root = build();

		if(maybe_ast_root.is_ok()){
			Node ast_root = maybe_ast_root.get_ok();

			// write program
			fs::path program_path = current_circuit_dir / ("circuit" + extension);
			std::ofstream stream(program_path.string());
			write(stream, ast_root);
			
			// write AST
			fs::path ast_path = current_circuit_dir / "circuit.ast";
			std::ofstream ast_stream(ast_path.string());
			ast_stream << ast_root << std::endl;

			// render graph for main circuit
			fs::path img_path = current_circuit_dir / "main_circ.png";
			context.maybe_render_qig();

			INFO("Program written to " << program_path.string());
			INFO("AST written to " << ast_path.string());

		} else {
        	ERROR(maybe_ast_root.get_error());
		}
    }
}
