#include <ast.h>

#include <sstream>
#include <result.h>

#include <block.h>
#include <gate.h>
#include <compound_stmts.h>
#include <float.h>
#include <float_list.h>
#include <resource_list.h>
#include <subroutine_op_args.h>
#include <resource_defs.h>
#include <qubit_op.h>
#include <gate_op_args.h>
#include <subroutines.h>
#include <compound_stmt.h>
#include <nested_branch.h>
#include <disjunction.h>
#include <conjunction.h>
#include <compare_op_bitwise_or_pair_child.h>
#include <expression.h>
#include <gate_name.h>

#include <generator.h>

std::string Node::indentation_tracker = "";


std::shared_ptr<Node> Ast::get_node(const std::shared_ptr<Node> parent, const Term& term){

	if(parent == nullptr){
		throw std::runtime_error(ANNOT("Node must have a parent!"));
	}

	if(term.is_syntax()){
		// std::cout << term.get_syntax() << std::endl;
		return std::make_shared<Node>(term.get_syntax());
	}

	U64 hash = term.get_hash();
	U8 scope = term.get_scope();

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

		/// TODO: add grammar syntax to allow certain rules to exclude other rules downstream, useful for non_comptime_block
		// case Common::non_comptime_block:
		// 	context.set_can_apply_subroutines(false);
		// 	return std::make_shared<Node>(str, hash);

		case Common::block:
			return context.new_block_node(str, hash);

		case Common::body:
			return std::make_shared<Node>(str, hash);

		case Common::compound_stmts:
			return context.get_compound_stmts(parent);
		
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
		
		// case Common::simple_stmt:
		// 	return std::make_shared<Simple_stmt>();

		case Common::circuit_id:
			return context.get_circuit_id();

		case Common::total_num_qubits:
			return std::make_shared<Integer>(context.get_max_external_qubits());

		case Common::main_circuit_name:				
			return std::make_shared<Variable>(Common::TOP_LEVEL_CIRCUIT_NAME);

		case Common::subroutine_defs:
			return context.new_subroutines_node();	

		case Common::qubit_op:
			return context.new_qubit_op_node();
	
		case Common::circuit_name:
			return std::make_shared<Variable>(context.get_current_block_owner());

		case Common::qreg_size:
			return context.get_current_qubit_definition_size();

		case Common::qubit_def_name:
			return context.get_current_qubit_definition_name();

		case Common::qubit_defs:
			return context.get_qubit_defs_node(scope);

		case Common::bit_defs:
			return context.get_bit_defs_node(scope);

		case Common::qubit_def:
			return context.new_qubit_definition(scope);

		case Common::bit_def:
			return context.new_bit_definition(scope);

		case Common::creg_size:
			return context.get_current_bit_definition_size();

		case Common::bit_def_name:
			return context.get_current_bit_definition_name();

		case Common::qubit_list: {

			unsigned int num_qubits;

			if(*parent == Common::subroutine_op_arg){
				num_qubits = context.get_current_gate()->get_next_qubit_def()->get_size()->get_num();
			} else {
				num_qubits = context.get_current_gate()->get_num_external_qubits();
			}

			return std::make_shared<Qubit_list>(num_qubits);
		}

		case Common::bit_list:
			return std::make_shared<Bit_list>(context.get_current_gate()->get_num_external_bits());

		case Common::float_list:
			return std::make_shared<Float_list>(context.get_current_gate()->get_num_floats());

		// (qu)bit_def_list and (qu)bit_def_size are a special cases used only for pytket->guppy conversion
		case Common::qubit_def_list:
			context.get_current_block()->qubit_def_pointer_reset();
			return std::make_shared<Node>(str, hash, Node_constraint(Common::qubit_def_size, context.get_current_block()->num_qubit_defs_of(EXTERNAL_SCOPE)));

		case Common::qubit_def_size:
			if (context.new_qubit_definition(OWNED_SCOPE)->is_register_def()) {
				return context.get_current_qubit_definition_size();
			} else {
				return std::make_shared<Integer>(0);
			}

		case Common::bit_def_list:
			context.get_current_block()->bit_def_pointer_reset();
			return std::make_shared<Node>(str, hash, Node_constraint(Common::bit_def_size, context.get_current_block()->num_bit_defs_of(EXTERNAL_SCOPE)));

		case Common::bit_def_size:
			if (context.new_bit_definition(EXTERNAL_SCOPE)->is_register_def()) {
				return context.get_current_bit_definition_size();
			} else {
				return std::make_shared<Integer>(0);
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
		
		case Common::float_literal:
			return std::make_shared<Float>();

		case Common::number:
			return std::make_shared<Integer>();

		case Common::gate_name:
			return std::make_shared<Gate_name>(parent, context.get_current_block(), swarm_testing_gateset);

		case Common::subroutine: {
			std::shared_ptr<Block> subroutine = context.get_random_block();
			
			/*
				create gate from subroutine
				- the hash of the node will be Common::subroutine, and the string will be the name of the block defining this subroutine
				- we can then use the hash later to detect which gate nodes are subroutines, and get their names by getting the string of the node 
			*/

			subroutine->print_info();

			return context.new_gate(subroutine->get_owner(), hash, subroutine->get_qubit_defs());
		}

		case Common::subroutine_op_args:
			return std::make_shared<Subroutine_op_args>(context.get_current_gate()->get_num_external_qubit_defs());

		case Common::subroutine_op_arg:
			return context.new_arg();
	
		case Common::h: case Common::x: case Common::y: case Common::z: case Common::t:
		case Common::tdg: case Common::s: case Common::sdg: case Common::project_z: case Common::measure_and_reset:
		case Common::v: case Common::vdg:
			return context.new_gate(str, hash, 1, 0, 0);

		case Common::cx : case Common::cy: case Common::cz: case Common::cnot:
		case Common::ch:
			return context.new_gate(str, hash, 2, 0, 0);

		case Common::crz:
			return context.new_gate(str, hash, 2, 0, 1);

		case Common::ccx: case Common::cswap: case Common::toffoli:
			return context.new_gate(str, hash, 3, 0, 0);

		case Common::u1: case Common::rx: case Common::ry: case Common::rz:
			return context.new_gate(str, hash, 1, 0, 1);

		case Common::u2:
			return context.new_gate(str, hash, 1, 0, 2);

		case Common::u3: case Common::u:
			return context.new_gate(str, hash, 1, 0, 3);
		
		case Common::Measure:
			return context.new_gate(str, hash, 1, 1, 0);
		
		case Common::barrier:{
			std::shared_ptr<Block> current_block = context.get_current_block();

			unsigned int n_qubits = std::min((unsigned int)WILDCARD_MAX, (unsigned int)current_block->num_qubits_of(ALL_SCOPES));
			unsigned int random_barrier_width = random_int(n_qubits, 1);

			return context.new_gate(str, hash, random_barrier_width, 0, 0);
		}

		case Common::gate_op_args:
			return std::make_shared<Gate_op_args>(context.get_current_gate());

		default:
			return std::make_shared<Node>(str, hash);
	}
}

void Ast::write_branch(std::shared_ptr<Node> parent, const Term& term){

	if(term.is_pointer()){

		Branch branch = term.get_rule()->pick_branch(parent);

		for(const Term& child_term : branch){
			
			std::shared_ptr<Node> child_node = get_node(parent, child_term);

			parent->add_child(child_node);

			if(child_node->get_num_children()) continue;

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
		context.set_genome(genome);

		Term term;
		term.set(entry);

		std::shared_ptr<Node> root = get_node(std::make_shared<Node>(""), term);

		write_branch(root, term);

		if(genome.has_value()){
			dag = genome.value().dag;
		} else {
			std::shared_ptr<Block> main_circuit_block = context.get_current_block();
			dag.make_dag(main_circuit_block);
		}

		context.print_block_info();

		std::cout << dag << std::endl;

		res.set_ok(*root);
	}

	return res;
}

Genome Ast::genome(){
	return Genome{.dag = dag, .dag_score = dag.score()};
}