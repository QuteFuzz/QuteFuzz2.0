#ifndef CONTEXT_H
#define CONTEXT_H

#include <block.h>
#include <resource_definition.h>
#include <variable.h>
#include <resource_defs.h>
#include <resource_list.h>
#include <arguments.h>
#include <arg.h>
#include <float_list.h>
#include <compound_stmt.h>
#include <compound_stmts.h>
#include <gate.h>
#include <gate_name.h>
#include <gate_op_kind.h>
#include <subroutines.h>
#include <genome.h>
#include <nested_stmt.h>
#include <nested_branch.h>

namespace Context {

    enum Level {
        PROGRAM,
		BLOCK,
        QUBIT_OP,
    };

    struct Context {

        public:
            Context(){}

			void reset(Level l);

			inline std::string get_current_block_owner(){
				return current_block_owner;
			}

            void set_can_apply_subroutines(bool flag = true);

            unsigned int get_max_external_qubits();

			unsigned int get_max_external_bits();

			std::shared_ptr<Block> get_current_block() const;

			std::shared_ptr<Block> get_random_block();

            std::shared_ptr<Block> new_block_node(std::string str, U64 hash);
            
			std::shared_ptr<Qubit_defs> get_qubit_defs_node(std::string str, U64 hash);

			std::shared_ptr<Bit_defs> get_bit_defs_node(std::string str, U64 hash);

			std::optional<std::shared_ptr<Block>> get_block(std::string owner);

			std::vector<std::shared_ptr<Block>> get_blocks() { return blocks;};

			inline std::shared_ptr<Resource::Qubit> new_qubit(){
				U8 scope = (*current_gate == Common::Measure) ? OWNED_SCOPE : ALL_SCOPES;

				Resource::Qubit* random_qubit = get_current_block()->get_random_qubit(scope); 

				random_qubit->extend_flow_path(current_qubit_op, current_port++);

				current_qubit = std::make_shared<Resource::Qubit>(*random_qubit);


				return current_qubit;
			}

			std::shared_ptr<Gate_op_kind> new_gate_op_kind_node(std::string str, U64 hash) {
				if (Common::cross_qss && genome.has_value()) {
					std::shared_ptr<Gate_op_kind> current_gate_op_kind = std::static_pointer_cast<Gate_op_kind>(current_qubit_op->find(Common::gate_op_kind));
					current_qubit_list = current_gate_op_kind->find(Common::qubit_list);
					current_float_list = current_gate_op_kind->find(Common::float_list);
					current_argument_list = (current_qubit_list == nullptr) ? current_gate_op_kind->find(Common::arguments) : nullptr;
					
					/*
						Weird workaround to accomodate swapped float_list and qubit_list positions in gate_op_kind
						between guppy and pytket
					*/
					if (current_gate_op_kind != nullptr && current_float_list == nullptr && current_argument_list == nullptr) {
						current_gate_op_kind->set_from_dag();
						return current_gate_op_kind;
					} else {
						#ifdef DEBUG
						if (current_gate_op_kind == nullptr) {
							ERROR("Gate_op_kind not found in DAG!");
						} 
						#endif
						return std::make_shared<Gate_op_kind>(str, hash, get_current_gate_num_params(), get_current_gate_num_bits(), get_current_gate_num_qubit_params());
					}

				} else {
					return std::make_shared<Gate_op_kind>(str, hash, get_current_gate_num_params(), get_current_gate_num_bits(), get_current_gate_num_qubit_params());
				}
			}

			std::shared_ptr<Qubit_list> new_qubit_list_node() {
				unsigned int num_qubits = get_current_gate_num_qubits();
				
				// Return the qubit list from the DAG. This only gets called for non-subroutine gates
				if (Common::cross_qss && genome.has_value() && current_qubit_list != nullptr) {
					std::shared_ptr<Qubit_list> qubit_list = std::static_pointer_cast<Qubit_list>(current_qubit_list);
					qubit_list->set_from_dag();
					return qubit_list;
				} else {
					return std::make_shared<Qubit_list>(num_qubits);
				}
			}

			std::shared_ptr<Float_list> new_float_list_node(std::string str, U64 hash) {
				unsigned int num_floats = get_current_gate_num_params();

				if (Common::cross_qss && genome.has_value() && current_float_list != nullptr) {
					std::shared_ptr<Float_list> float_list = std::static_pointer_cast<Float_list>(current_float_list);
					float_list->set_from_dag();
					return float_list;
				} else {
					return std::make_shared<Float_list>(str, hash, num_floats);
				}
			}

			std::shared_ptr<Variable> get_current_qubit_name();

			std::shared_ptr<Integer> get_current_qubit_index();

			inline std::shared_ptr<Resource::Bit> new_bit(){
				Resource::Bit* random_bit = get_current_block()->get_random_bit();
				current_bit = std::make_shared<Resource::Bit>(*random_bit);
				
				return current_bit;
			}

			std::shared_ptr<Variable> get_current_bit_name();

			std::shared_ptr<Integer> get_current_bit_index();

			inline std::shared_ptr<Arg> new_arg(){
				if((current_gate != nullptr) && current_gate->is_subroutine_gate()){
					std::shared_ptr<Block> subroutine = get_block(current_gate->get_string()).value();
					std::shared_ptr<Resource_definition> qubit_def = subroutine->get_next_qubit_def(EXTERNAL_SCOPE);

					current_arg = std::make_shared<Arg>(qubit_def);
				} else {
					ERROR("Current gate is null or not a subroutine gate!");
				}

				return current_arg;
			}

			/// @brief This only gets called when creating qubit lists for subroutines in cross-qss mode, or guppy subroutines
			std::shared_ptr<Arguments> new_arguments_node() {
				if (Common::cross_qss && genome.has_value() && current_gate->is_subroutine_gate() && current_argument_list != nullptr) {
					std::shared_ptr<Arguments> arguments = std::static_pointer_cast<Arguments>(current_argument_list);
					arguments->set_from_dag();
					return arguments;
				} else {
					WARNING("Getting subroutine with no DAG " + current_gate->get_string());
					return std::make_shared<Arguments>(get_current_gate_num_qubit_params());
				}
			}

			inline std::shared_ptr<Arg> get_current_arg(){
				return current_arg;
			}

			inline std::shared_ptr<Qubit_definition> new_qubit_definition(U8 scope_filter = ALL_SCOPES){
				current_qubit_definition = get_current_block()->get_next_qubit_def(scope_filter);
				return current_qubit_definition;
			}

			std::shared_ptr<Variable> get_current_qubit_definition_name();

			std::shared_ptr<Integer> get_current_qubit_definition_size();

			inline std::shared_ptr<Bit_definition> new_bit_definition(U8 scope_filter = ALL_SCOPES){
				current_bit_definition = get_current_block()->get_next_bit_def(scope_filter);
				return current_bit_definition;
			}
			
			std::shared_ptr<Variable> get_current_bit_definition_name();

			std::shared_ptr<Integer> get_current_bit_definition_size();

			inline std::shared_ptr<Gate> new_gate(const std::string& str, int num_qubits, int num_bits, int num_params, U64 hash = 0ULL, int num_qubit_params = 0){

				current_gate = std::make_shared<Gate>(str, hash, num_qubits, num_bits, num_params, num_qubit_params);

				current_qubit_op->set_gate_node(current_gate);

				return current_gate;

				/*
					Don't need to consider Common::cross_qss since gate names should be already be copied wholesale for subroutines and gate string
					should be conforming to QSS standard by this point (upper/lower case)
				*/
				
			}

			int get_current_gate_num_params();

			int get_current_gate_num_qubit_params();

			int get_current_gate_num_qubits();

			int get_current_gate_num_bits();

			/// @brief Make barrier gate. The number of qubits used to choose the barrier width is a max of internal and external qubits, 
			/// because a block could have either or both
			/// @return 
			inline std::shared_ptr<Gate> get_barrier(){
				std::shared_ptr<Block> current_block = get_current_block();

				unsigned int n_qubits = std::min((unsigned int)WILDCARD_MAX, (unsigned int)current_block->total_num_qubits());
				unsigned int random_barrier_width = random_int(n_qubits, 1);

				return new_gate("barrier", random_barrier_width, 0, 0);
			}

			std::shared_ptr<Nested_branch> get_nested_branch(const std::string& str, const U64& hash, std::shared_ptr<Node> parent);

			std::shared_ptr<Nested_stmt> get_nested_stmt(const std::string& str, const U64& hash, std::shared_ptr<Node> parent);

			std::shared_ptr<Compound_stmt> get_compound_stmt(std::shared_ptr<Node> parent);

			std::shared_ptr<Compound_stmts> get_compound_stmts(std::shared_ptr<Node> parent);

			std::shared_ptr<Subroutines> new_subroutines_node();

			std::shared_ptr<Qubit_op> new_qubit_op_node(){
				reset(QUBIT_OP);

				if (Common::cross_qss && genome.has_value()) {

					/*
						current_qubit_op will be set to the DAG, while a new qubit_op is passed back. This allows subsequent grammar rules
						that are its children to also have access to the DAG without advancing to the next qubit_op
					*/
					if (current_block_is_subroutine()) {
						current_qubit_op = current_subroutine_dag->get_next_node();
					} else {
						current_qubit_op = genome.value().dag.get_next_node();
					}

					return Qubit_op::qubit_op_from_dag(current_qubit_op);

				} else {
					current_qubit_op = can_copy_dag ? genome.value().dag.get_next_node() : std::make_shared<Qubit_op>(get_current_block());
					return current_qubit_op;
					// current_qubit_op = std::make_shared<Qubit_op>(get_current_block());
				}
				
			}

			/// @brief  Make a gate_name node with constraints added based on the genome DAG's qubit_op node
			/// @param parent
			/// @return gate_name node straight from DAG
			std::shared_ptr<Node> make_gate_name(const std::shared_ptr<Node> parent, const std::optional<Node_constraint>& swarm_testing_gateset) {
				// Gate name is one of the children of the child of the DAG qubit_op
				if (Common::cross_qss && genome.has_value()) {

					bool is_subroutine = current_qubit_op->find(Common::subroutine_op) != nullptr;
					std::shared_ptr<Node> current_gate_op = is_subroutine ? current_qubit_op->find(Common::subroutine_op) : current_qubit_op->find(Common::gate_op); 
					/*
						Assume only 1 child to qubit_op, which is either gate_op or subroutine_op
					*/ 
					if (current_gate_op != nullptr && current_gate_op->find(Common::gate_name) != nullptr && !is_subroutine) {
						return std::make_shared<Gate_name>(Common::Rule_hash(current_gate_op->find(Common::gate_name)->get_children().at(0)->get_hash()));
						/*
							Get the gate hash and convert to gate name since different QSSes have different upper/lower case names or even entirely different names.
							Exceptions are subroutines since its named locally
						*/
					} else if (current_gate_op != nullptr && current_gate_op->find(Common::gate_name) != nullptr && is_subroutine) { 
						std::shared_ptr<Gate_name> subroutine_gate_name = std::static_pointer_cast<Gate_name>(current_gate_op->find(Common::gate_name));
						subroutine_gate_name->set_from_dag();
						return subroutine_gate_name;
						/*
							Copy subroutine gate name directly since naming doesn't change across QSS
						*/
					} else {
						WARNING("Could not find gate_op or subroutine_op in qubit_op children! Generating random gate name instead");
						return std::make_shared<Gate_name>(parent, get_current_block(), Common::swarm_testing ? swarm_testing_gateset : std::nullopt);
					}
				}

				return std::make_shared<Gate_name>(parent, get_current_block(), Common::swarm_testing ? swarm_testing_gateset : std::nullopt);
			};

			/// @brief Is the current block being generated a subroutine?
			/// @return 
			inline bool current_block_is_subroutine(){
                return subroutines_node.has_value() && (subroutines_node.value()->build_state() == NB_BUILD);
            }

			inline void set_ast_counter(const int& counter){ast_counter = counter;}

			inline std::shared_ptr<Integer> get_circuit_id(){return std::make_shared<Integer>(ast_counter);}

			void set_genome(const std::optional<Genome>& _genome);

			inline void print_block_info() const {		
				for(const std::shared_ptr<Block>& block : blocks){
					block->print_info();
				}
			}

        private:
			std::string current_block_owner;
            std::vector<std::shared_ptr<Block>> blocks;
			
			Block dummy_block;
			Integer dummy_int;
			Variable dummy_var;

            unsigned int subroutine_counter = 0;
			unsigned int ast_counter = 0;
			unsigned int current_port;
	        unsigned int nested_depth;
			unsigned int current_qubit_index = 0;
		
			std::shared_ptr<Qubit_definition> current_qubit_definition;
			std::shared_ptr<Bit_definition> current_bit_definition;
			std::shared_ptr<Resource::Qubit> current_qubit;
			std::shared_ptr<Resource::Bit> current_bit;
			std::shared_ptr<Gate> current_gate;
			std::shared_ptr<Qubit_op> current_qubit_op;
			std::shared_ptr<Arg> current_arg;
			std::shared_ptr<Node> current_qubit_list;
			std::shared_ptr<Node> current_float_list;
			std::shared_ptr<Node> current_argument_list;

			std::optional<std::shared_ptr<Subroutines>> subroutines_node = std::nullopt;
			std::optional<Genome> genome;
			std::optional<Dag::Dag> current_subroutine_dag;

			bool can_copy_dag;
    };

}

#endif
