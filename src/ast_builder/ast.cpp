#include <ast.h>

#include <result.h>
#include <block.h>
#include <gate.h>
#include <compound_stmts.h>
#include <simple_stmt.h>
#include <float.h>
#include <float_list.h>
#include <qubit_list.h>
#include <qubit_defs.h>
#include <qubit_op.h>
#include <gate_op_kind.h>
#include <subroutines.h>

std::string Node::indentation_tracker = "";

std::shared_ptr<Node> Ast::get_node_from_term(const Term& term){

	if(term.is_syntax()){
		return std::make_shared<Node>(term.get_syntax());

	} else {
	
		U64 hash = term.get_hash();
		std::string str = term.get_string();

		switch(hash){

			case Common::indent:
				Node::indentation_tracker += "\t";
				return dummy;

			case Common::dedent:
				if(Node::indentation_tracker.size()){
					Node::indentation_tracker.pop_back();
				}

				return dummy;

			case Common::block: case Common::main_block:
				return context.setup_block(str, hash);

			case Common::body: {
				return std::make_shared<Node>(str, hash);
			}

			case Common::compound_stmts:
				// qig set after all definitions have been made
				context.set_qig();
				// internal qubit defs is set after body: this is better catch-all point for checking
				context.set_can_apply_subroutines();
				return std::make_shared<Compound_stmts>(str, hash, WILDCARD_MAX);
			
			case Common::simple_stmt:
				return std::make_shared<Simple_stmt>(str, hash);

			case Common::circuit_id:
				return std::make_shared<Integer>(std::to_string(build_counter));

			case Common::main_circuit_name:				
				return std::make_shared<Variable>(Common::TOP_LEVEL_CIRCUIT_NAME);
				
			case Common::subroutines: {
				std::shared_ptr<Subroutines> node = std::make_shared<Subroutines>(str, hash);
				
				context.set_subroutines_node(node);

				return node;
			}

			case Common::gate_op_kind:
				return std::make_shared<Gate_op_kind>(str, hash, context.get_current_gate_num_params());

			case Common::qubit_op:
				context.reset(Context::QUBIT_OP);
				return std::make_shared<Qubit_op>(str, hash, context.get_current_block()->get_can_apply_subroutines());

			case Common::subroutine: {				
				std::shared_ptr<Block> subroutine = context.get_random_block();
				int num_sub_qubits = subroutine->num_external_qubits();

				return context.get_current_gate(subroutine->get_owner(), num_sub_qubits, 0);				
			}
	
			case Common::circuit_name:
				return std::make_shared<Variable>(context.get_current_block_owner());

			case Common::qreg_size:
				return context.get_current_qubit_definition_size();

			case Common::qreg_name:
				return context.get_current_qubit_definition_name();

			case Common::qubit_def_external: case Common::qubit_def_internal:
				return context.get_current_qubit_definition();

			case Common::qubit_defs_external: case Common::qubit_defs_internal:
				return context.make_qubit_definitions(str, hash);

			case Common::discard_internal_qubits: {
				context.get_current_block()->qubit_def_pointer_reset();
				size_t num_internal_qubit_defs = context.get_current_block()->num_internal_qubit_defs();
				return context.discard_qubit_defs(str, hash, num_internal_qubit_defs);
			}
			
			case Common::discard_internal_qubit:
				return context.get_current_qubit_definition_discard(str, hash);
			
			case Common::qubit_list: {
				size_t num_qubits = context.get_current_gate_num_qubits();
				return std::make_shared<Qubit_list>(str, hash, num_qubits);
			}

			case Common::qubit_index:
				return context.get_current_qubit_index();

			case Common::qubit_name:
				return context.get_current_qubit_name();

			case Common::qubit: 
				return context.get_current_qubit();

			case Common::float_list: {
				size_t num_floats = context.get_current_gate_num_params();
				return std::make_shared<Float_list>(str, hash, num_floats);
			}

			case Common::float_literal:
				return std::make_shared<Float>();
	
			case Common::h: case Common::x: case Common::y: case Common::z: {
				return context.get_current_gate(str, 1, 0);
			}

			case Common::cx : case Common::cy: case Common::cz: case Common::cnot: {
				return context.get_current_gate(str, 2, 0);
			}

			case Common::ccx: case Common::cswap: case Common::toffoli:{
				return context.get_current_gate(str, 3, 0);
			}

			case Common::u1: case Common::rx: case Common::ry: case Common::rz:{
				return context.get_current_gate(str, 1, 1);
			}

			case Common::u2:{
				return context.get_current_gate(str, 1, 2);
			}

			case Common::u3: case Common::u:{
				return context.get_current_gate(str, 1, 3);
			}

			default:
				return std::make_shared<Node>(str, hash);
		}
	}
}

void Ast::write_branch(std::shared_ptr<Node> parent, const Term& term){

	if(term.is_pointer()){

		Branch branch = term.get_rule()->pick_branch(parent);

        for(size_t i = 0; i < branch.size(); i++){
			
			Term child_term = branch.at(i);
			
			std::shared_ptr<Node> child_node = get_node_from_term(child_term);

			parent->add_child(child_node);

			write_branch(child_node, child_term);
        }

	}

	// done
	parent->transition_to_done();
}

Result<Node> Ast::build(){
	Result<Node> res;

	if(entry == nullptr){
		res.set_error("Entry point not set");

	} else {
		context.reset(Context::PROGRAM);

		Term entry_as_term;
		entry_as_term.set(entry);

		std::shared_ptr<Node> root_ptr = get_node_from_term(entry_as_term);

		write_branch(root_ptr, entry_as_term);

		res.set_ok(*root_ptr);
	}

	return res;
}

void Ast::ast_to_program(fs::path output_dir, const std::string& extension, int num_programs){

	for(build_counter = 0; build_counter < num_programs; build_counter++){
		fs::path current_circuit_dir =  output_dir / ("circuit" + std::to_string(build_counter));
		fs::create_directory(current_circuit_dir);

		context.set_circuits_dir(current_circuit_dir);
		
	    Result<Node> maybe_ast_root = build();

		if(maybe_ast_root.is_ok()){
			Node ast_root = maybe_ast_root.get_ok();

			// write program
			fs::path program_path = current_circuit_dir / ("circuit" + extension);
			std::ofstream stream(program_path.string());
			
			stream << ast_root << std::endl;

			INFO("Program written to " + program_path.string());
			
			// render AST
			// render_ast(ast_root, current_circuit_dir);

			// render QIG for main
			context.render_qig();

		} else {
        	ERROR(maybe_ast_root.get_error());
		}
    }
}

void Ast::render_ast(const Node& root, const fs::path& current_circuit_dir){

	std::string dot_string;

	dot_string += "digraph AST {\n";

	root.make_dot_string(dot_string);

    dot_string += "}\n";

    // render graph
	fs::path ast_path = current_circuit_dir / "ast.png";

    const std::string str = ast_path.string();
    std::string command = "dot -Tpng -o " + str;
    
    pipe_to_command(command, dot_string);
    INFO("AST rendered to " + ast_path.string());
}

