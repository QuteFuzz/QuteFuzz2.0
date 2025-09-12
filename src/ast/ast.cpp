#include <ast.h>

#include <sstream>
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
#include <nested_branch.h>
#include <disjunction.h>
#include <conjunction.h>
#include <compare_op_bitwise_or_pair_child.h>
#include <expression.h>
#include <gate_name.h>
#include <discard_internal_qubits.h>
#include <discard_internal_qubit.h>

#include <generator.h>

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

		case Common::block: case Common::main_block:
			return context.new_block_node(str, hash);

		case Common::non_comptime_block:
			context.set_can_apply_subroutines(false);
			return std::make_shared<Node>(str, hash);

		case Common::compound_stmts:
			return context.get_compound_stmts(parent);

		case Common::arguments: {
			return context.new_arguments_node();
		}

		case Common::arg:
			return context.new_arg();

		case Common::arg_singular_qubit:
			return std::make_shared<Qubit_list>(1);

		case Common::arg_register_qubits:
			return std::make_shared<Qubit_list>(context.get_current_arg()->get_qubit_def_size());
		
		case Common::compound_stmt:
			return context.get_compound_stmt(parent);

		case Common::if_stmt:
			return context.get_nested_stmt(str, hash, parent);

		case Common::elif_stmt: case Common::else_stmt:
			return context.get_nested_branch(str, hash, parent);

		case Common::disjunction:
			return std::make_shared<Disjunction>();

		case Common::conjunction:
			return std::make_shared<Conjunction>();

		case Common::expression:
			return std::make_shared<Expression>();
		
		case Common::simple_stmt:
			return std::make_shared<Simple_stmt>();

		case Common::circuit_id:
			return context.get_circuit_id();

		case Common::total_num_qubits: {
			return std::make_shared<Integer>(context.get_max_external_qubits());
		}

		case Common::main_circuit_name:				
			return std::make_shared<Variable>(Common::TOP_LEVEL_CIRCUIT_NAME);
			
		case Common::subroutines:
			return context.new_subroutines_node();			

		case Common::gate_op_kind:
			return context.new_gate_op_kind_node(str, hash);

		case Common::qubit_op:
			return context.new_qubit_op_node();
	
		case Common::circuit_name:
			return std::make_shared<Variable>(context.get_current_block_owner());

		case Common::qreg_size:
			return context.get_current_qubit_definition_size();

		case Common::qubit_def_name:
			return context.get_current_qubit_definition_name();

		case Common::qubit_def_external: case Common::qubit_def_internal: {
			U8 scope_filter = ALL_SCOPES;

			if (*parent == Common::qubit_defs_external_owned || *parent == Common::qubit_defs_internal) {
				scope_filter = OWNED_SCOPE;
			}

			return context.new_qubit_definition(scope_filter);
		}

		case Common::qubit_defs_external: case Common::qubit_defs_internal:
		case Common::qubit_defs_external_owned:
			return context.get_qubit_defs_node(str, hash);

		case Common::creg_size:
			return context.get_current_bit_definition_size();

		case Common::bit_def_name:
			return context.get_current_bit_definition_name();

		case Common::bit_def_external: case Common::bit_def_internal:
			return context.new_bit_definition();

		case Common::bit_defs_external: case Common::bit_defs_internal:
			return context.get_bit_defs_node(str, hash);

		/*
			go through the qubit definitions and discard them
			1. reset pointer, such that calls to `new_qubit_definition` traverse the definitions in order of definition
			2. Set constraint on number of `discard_internal_qubit` branches, only owned qubits are considered
			3. Traverse definitions, returning only owned definitions
			==============================================================
		*/
		case Common::discard_internal_qubits: {
			context.get_current_block()->qubit_def_pointer_reset();
			unsigned int num_owned_qubit_defs = context.get_current_block()->num_owned_qubit_defs();

			return std::make_shared<Discard_internal_qubits>(num_owned_qubit_defs);			
		}
		
		case Common::discard_internal_qubit:
			return std::make_shared<Discard_internal_qubit>(context.new_qubit_definition(OWNED_SCOPE), context.get_current_block_owner());

		//	===============================================================

		case Common::qubit_list: {
			return context.new_qubit_list_node();
		}

		case Common::bit_list: {
			unsigned int num_bits = context.get_current_gate_num_bits();
			return std::make_shared<Bit_list>(num_bits);
		}

		// (qu)bit_def_list and (qu)bit_def_size are a special cases used only for pytket->guppy conversion
		case Common::qubit_def_list:
			context.get_current_block()->qubit_def_pointer_reset();
			return std::make_shared<Node>(str, hash, Node_constraint(Common::qubit_def_size, context.get_current_block()->num_owned_qubit_defs()));

		case Common::qubit_def_size: {
			if (context.new_qubit_definition(OWNED_SCOPE)->is_register_def()) {
				return context.get_current_qubit_definition_size();
			} else {
				return std::make_shared<Integer>(0);
			}
		}

		case Common::bit_def_list:
			context.get_current_block()->bit_def_pointer_reset();
			return std::make_shared<Node>(str, hash, Node_constraint(Common::bit_def_size, context.get_current_block()->num_external_bit_defs()));

		case Common::bit_def_size: {
			if (context.new_bit_definition(EXTERNAL_SCOPE)->is_register_def()) {
				return context.get_current_bit_definition_size();
			} else {
				return std::make_shared<Integer>(0);
			}
		}

		case Common::qubit_index:
			return context.get_current_qubit_index();

		case Common::qubit_name:
			return context.get_current_qubit_name();

		case Common::qubit:
			return context.new_qubit();

		case Common::bit_index:
			return context.get_current_bit_index();

		case Common::bit_name:
			return context.get_current_bit_name();

		case Common::bit:
			return context.new_bit();
		
		case Common::float_list: {
			return context.new_float_list_node(str, hash);
		}

		case Common::float_literal:
			return std::make_shared<Float>();

		case Common::number:
			return std::make_shared<Integer>();

		case Common::gate_name: {
			return context.make_gate_name(parent, swarm_testing_gateset);
		}

		case Common::subroutine: {
			std::shared_ptr<Block> subroutine = context.get_random_block();
			int num_sub_qubits = subroutine->num_external_qubits();
			int num_sub_bits = subroutine->num_external_bits();

			subroutine->qubit_def_pointer_reset();

			return context.new_gate(subroutine->get_owner(), num_sub_qubits, num_sub_bits, 0, hash, subroutine->num_external_qubit_defs());
			/*
				num_params means different things for guppy and pytket
				- for guppy, it means the number of qubits the subroutine takes
				- for pytket, it means the number of floats a gate takes
				Therefore, I've added a new parameter to specify specifically the number of qubits that are taken in as parameters
			*/
		}

		case Common::h: case Common::x: case Common::y: case Common::z: case Common::t:
		case Common::tdg: case Common::s: case Common::sdg: case Common::project_z: case Common::measure_and_reset:
		case Common::v: case Common::vdg:
			return context.new_gate(str, 1, 0, 0, hash);

		case Common::cx : case Common::cy: case Common::cz: case Common::cnot:
		case Common::ch:
			return context.new_gate(str, 2, 0, 0, hash);

		case Common::crz:
			return context.new_gate(str, 2, 0, 1, hash);

		case Common::ccx: case Common::cswap: case Common::toffoli:
			return context.new_gate(str, 3, 0, 0, hash);

		case Common::u1: case Common::rx: case Common::ry: case Common::rz:
			return context.new_gate(str, 1, 0, 1, hash);

		case Common::u2:
			return context.new_gate(str, 1, 0, 2, hash);

		case Common::u3: case Common::u:
			return context.new_gate(str, 1, 0, 3, hash);
		
		case Common::Measure:
			return context.new_gate(str, 1, 1, 0, hash);

		/* Added hash to every new_gate instance. Not sure if this is intended but it is needed for swarm testing */
		
		case Common::barrier:
			return context.get_barrier();

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

			if(child_node->is_from_dag()) continue;

			write_branch(child_node, child_term);
        }

	}

	// done
	parent->transition_to_done();
}

Result<Node> Ast::build(const std::optional<Genome>& genome, std::optional<Node_constraint>& _swarm_testing_gateset){
	Result<Node> res;

	if(entry == nullptr){
		res.set_error("Entry point not set");

	} else {

		swarm_testing_gateset = _swarm_testing_gateset;

		context.reset(Context::PROGRAM);

		Term entry_as_term;
		entry_as_term.set(entry);

		std::shared_ptr<Node> root_ptr = get_node_from_term(dummy, entry_as_term);

		context.set_genome(genome);

		write_branch(root_ptr, entry_as_term);

		if(genome.has_value()){
			dag = genome.value().dag;
			subroutine_dags = genome.value().subroutine_dags;
		} else {
			//Make main dag
			std::shared_ptr<Block> current_block = context.get_current_block();
			dag.make_dag(current_block->get_qubits(), current_block->get_bits(), current_block->get_owner());
			
			// Iterate through the blocks and create subroutine dags
			subroutine_dags.clear();
			for (auto block : context.get_blocks()) {
				if (block->get_owner() != Common::TOP_LEVEL_CIRCUIT_NAME) {
					Dag::Dag sub_dag;
					sub_dag.make_dag(block->get_qubits(), block->get_bits(), block->get_owner());
					subroutine_dags.push_back(sub_dag);
				}
			}
		}

		context.print_block_info();

		res.set_ok(*root_ptr);
	}

	return res;
}

Genome Ast::genome(){
	return Genome{.dag = dag, .subroutine_dags = subroutine_dags, .dag_score = dag.score()};
}