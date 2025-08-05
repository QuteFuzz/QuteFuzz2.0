#include <ast.h>

#include <result.h>
#include <block.h>
#include <gate.h>
#include <compound_stmts.h>
#include <simple_stmt.h>
#include <float.h>
#include <float_list.h>
#include <resource_list.h>
#include <arguments.h>
#include <resource_defs.h>
#include <qubit_op.h>
#include <gate_op_kind.h>
#include <subroutines.h>
#include <compound_stmt.h>
#include <control_flow_branch.h>
#include <disjunction.h>
#include <conjunction.h>
#include <compare_op_bitwise_or_pair_child.h>
#include <expression.h>

std::string Node::indentation_tracker = "";

/// @brief Given a term, return a node for that term. `parent` is guaranteed to never be `nullptr` but runtime error thrown incase not true
/// @param parent 
/// @param term 
/// @return 
std::shared_ptr<Node> Ast::get_node_from_term(const std::shared_ptr<Node> parent, const Term& term){

	if(parent == nullptr){
		throw std::runtime_error(ANNOT("Node must have a parent!"));
	}

	if(term.is_syntax()){
		return std::make_shared<Node>(term.get_syntax());
	}

	U64 hash = term.get_hash();
	std::string str = term.get_string();

	if(*parent == Common::compare_op_bitwise_or_pair){
		return std::make_shared<Compare_op_bitwise_or_pair_child>(str, hash);
	}
	
	switch(hash){

		case Common::indent:
			Node::indentation_tracker += "\t";
			return dummy;

		case Common::dedent:
			if(Node::indentation_tracker.size()){
				Node::indentation_tracker.pop_back();
			}

			return dummy;

		case Common::block: case Common::main_block: {
			context.reset(Context::BLOCK);
			return context.setup_block(str, hash);
		}

		case Common::non_comptime_block:
			context.set_can_apply_subroutines(false);
			return std::make_shared<Node>(str, hash);

		case Common::body:
			return std::make_shared<Node>(str, hash);

		case Common::compound_stmts:
			if(*parent == Common::body){
				context.set_can_apply_subroutines();
			}

			return std::make_shared<Compound_stmts>(str, hash, WILDCARD_MAX);
		
		case Common::arguments: {
			context.set_current_gate_definition();
			size_t num_args = context.get_current_gate_num_params();
			return std::make_shared<Arguments>(str, hash, num_args);
		}

		case Common::arg: {
			context.set_current_arg(str, hash);
			return context.get_current_arg();
		}

		case Common::arg_singular_qubit: {
			return std::make_shared<Resource_list::Qubit_list>(str, hash, 1);
		}

		case Common::arg_register_qubits: {
			return std::make_shared<Resource_list::Qubit_list>(str, hash, context.get_current_arg()->get_qubit_def_size());
		}
		
		case Common::compound_stmt:
			return context.get_compound_stmt(str, hash);

		case Common::if_stmt:
			return context.get_control_flow_stmt(str, hash);

		case Common::elif_stmt: case Common::else_stmt:
			return std::make_shared<Control_flow_branch>(str, hash);

		case Common::disjunction:
			return std::make_shared<Disjunction>(str, hash);

		case Common::conjunction:
			return std::make_shared<Conjunction>(str, hash);

		case Common::expression:
			return std::make_shared<Expression>(str, hash);
		
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

		case Common::qubit_op: {
			context.reset(Context::QUBIT_OP);
			return std::make_shared<Qubit_op>(str, hash, context.get_current_block()->get_can_apply_subroutines(), context.get_current_block()->num_internal_qubits() > 0);
		}
	
		case Common::circuit_name:
			return std::make_shared<Variable>(context.get_current_block_owner());

		case Common::qreg_size:
			return context.get_current_qubit_definition_size();

		case Common::qubit_def_name:
			return context.get_current_qubit_definition_name();

		case Common::qubit_def_external: case Common::qubit_def_internal:
			context.set_current_qubit_definition();
			return context.get_current_qubit_definition();

		case Common::qubit_defs_external: case Common::qubit_defs_internal:
			return context.make_qubit_definitions(str, hash);

		case Common::creg_size:
			return context.get_current_bit_definition_size();

		case Common::bit_def_name:
			return context.get_current_bit_definition_name();

		case Common::bit_def_external: case Common::bit_def_internal:
			context.set_current_bit_definition();
			return context.get_current_bit_definition();

		case Common::bit_defs_external: case Common::bit_defs_internal:
			return context.make_bit_definitions(str, hash);

		case Common::discard_internal_qubits: {
			context.get_current_block()->qubit_def_pointer_reset();
			size_t num_internal_qubit_defs = context.get_current_block()->num_internal_qubit_defs();
			return context.get_discard_qubit_defs(str, hash, num_internal_qubit_defs);
		}
		
		case Common::discard_internal_qubit:
			context.set_current_qubit_definition_owned();
			return context.get_current_qubit_definition_discard(str, hash);
		
		case Common::qubit_list: {
			size_t num_qubits = context.get_current_gate_num_qubits();
			return std::make_shared<Resource_list::Qubit_list>(str, hash, num_qubits);
		}

		case Common::bit_list: {
			size_t num_bits = context.get_current_gate_num_bits();
			return std::make_shared<Resource_list::Bit_list>(str, hash, num_bits);
		}

		// qubit_def_list and qubit_def_size are a special cases used only for pytket->guppy conversion
		case Common::qubit_def_list: {
			context.get_current_block()->qubit_def_pointer_reset();
			return std::make_shared<Node>(str, hash, Node_constraint(Common::qubit_def_size, context.get_current_block()->num_external_qubit_defs()));
		}

		case Common::qubit_def_size: {
			return context.get_current_qubit_definition_size_0_indexed();
		}

		case Common::qubit_index:
			return context.get_current_qubit_index();

		case Common::qubit_name:
			return context.get_current_qubit_name();

		case Common::qubit: 
			context.set_current_qubit();
			return context.get_current_qubit();

		case Common::bit_index:
			return context.get_current_bit_index();

		case Common::bit_name:
			return context.get_current_bit_name();

		case Common::bit:
			context.set_current_bit();
			return context.get_current_bit();
		
		case Common::float_list: {
			size_t num_floats = context.get_current_gate_num_params();
			return std::make_shared<Float_list>(str, hash, num_floats);
		}

		case Common::float_literal:
			return std::make_shared<Float>();

		case Common::number:
			return std::make_shared<Integer>();

		case Common::subroutine: {				
			std::shared_ptr<Block> subroutine = context.get_random_block();
			int num_sub_qubits = subroutine->num_external_qubits();
			int num_sub_bits = subroutine->num_external_bits();

			subroutine->qubit_def_pointer_reset();
			context.set_current_gate(subroutine->get_owner(), num_sub_qubits, num_sub_bits, subroutine->num_external_qubit_defs());				
		
			return context.get_current_gate();
		}

		case Common::h: case Common::x: case Common::y: case Common::z: case Common::t:
		case Common::tdg: case Common::s: case Common::sdg: case Common::project_z: case Common::measure_and_reset:
			context.set_current_gate(str, 1, 0, 0);
			return context.get_current_gate();

		case Common::cx : case Common::cy: case Common::cz: case Common::cnot:
		case Common::ch:
			context.set_current_gate(str, 2, 0, 0);
			return context.get_current_gate();

		case Common::crz:
			context.set_current_gate(str, 2, 0, 1);
			return context.get_current_gate();

		case Common::ccx: case Common::cswap: case Common::toffoli:
			context.set_current_gate(str, 3, 0, 0);
			return context.get_current_gate();

		case Common::u1: case Common::rx: case Common::ry: case Common::rz:
			context.set_current_gate(str, 1, 0, 1);
			return context.get_current_gate();

		case Common::u2:
			context.set_current_gate(str, 1, 0,2);
			return context.get_current_gate();

		case Common::u3: case Common::u:
			context.set_current_gate(str, 1, 0, 3);
			return context.get_current_gate();

		default:
			return std::make_shared<Node>(str, hash);
	}
}

void Ast::write_branch(std::shared_ptr<Node> parent, const Term& term){

	if(term.is_pointer()){

		Branch branch = term.get_rule()->pick_branch(parent);

        for(size_t i = 0; i < branch.size(); i++){
			
			Term child_term = branch.at(i);
			
			std::shared_ptr<Node> child_node = get_node_from_term(parent, child_term);

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

		std::shared_ptr<Node> root_ptr = get_node_from_term(dummy, entry_as_term);

		write_branch(root_ptr, entry_as_term);

		res.set_ok(*root_ptr);
	}

	return res;
}

void Ast::ast_to_program(fs::path output_dir, const std::string& extension, int num_programs){

	for(build_counter = 0; build_counter < num_programs; build_counter++){
		fs::path current_circuit_dir =  output_dir / ("circuit" + std::to_string(build_counter));
		fs::create_directory(current_circuit_dir);
		
	    Result<Node> maybe_ast_root = build();

		if(maybe_ast_root.is_ok()){
			Node ast_root = maybe_ast_root.get_ok();

			fs::path program_path = current_circuit_dir / ("circuit" + extension);
			std::ofstream stream(program_path.string());

			// render dag (main block)
			if (Common::render_dags) {
				render_dag(current_circuit_dir);
			}

			int dag_score = get_dag_score();

			INFO("Dag score: " + std::to_string(dag_score));

			// write program
			stream << ast_root << std::endl;
			INFO("Program written to " + program_path.string());
			
		} else {
        	ERROR(maybe_ast_root.get_error());
		}
    }
}

int Ast::get_dag_score(){
	return Dag::Heuristics(context.get_current_block()->get_qubits(), context.get_current_block()->get_bits()).score();
}

void Ast::render_dag(const fs::path& current_circuit_dir){
    std::ostringstream dot_string;

    dot_string << "digraph G {\n";

    for(const Resource::Resource& qubit : context.get_current_block()->get_qubits()){
        qubit.extend_dot_string(dot_string);
    }

	for(const Resource::Resource& bit : context.get_current_block()->get_bits()){
		bit.extend_dot_string(dot_string);
	}

    dot_string << "}\n";

    fs::path dag_render_path = current_circuit_dir / "dag.png";

    const std::string str = dag_render_path.string();
    std::string command = "dot -Tpng -o " + str;
    
    pipe_to_command(command, dot_string.str());
    INFO("Program DAG rendered to " + dag_render_path.string());
}