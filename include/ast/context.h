#ifndef CONTEXT_H
#define CONTEXT_H

#include <block.h>
#include <resource_definition.h>
#include <variable.h>
#include <resource_defs.h>
#include <arg.h>
#include <compound_stmt.h>
#include <compound_stmts.h>
#include <gate.h>
#include <subroutines.h>
#include <genome.h>

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

			inline std::shared_ptr<Resource::Qubit> new_qubit(){
				U8 scope = (*current_gate == Common::Measure) ? OWNED_SCOPE : ALL_SCOPES;

				Resource::Qubit* random_qubit = get_current_block()->get_random_qubit(scope); 
				
				random_qubit->extend_flow_path(current_qubit_op, current_port++);

				current_qubit = std::make_shared<Resource::Qubit>(*random_qubit);

				return current_qubit;
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
					std::shared_ptr<Resource_definition> qubit_def = subroutine->get_next_qubit_def(EXTERNAL_SCOPE | OWNED_SCOPE);

					current_arg = std::make_shared<Arg>(qubit_def);
				}

				return current_arg;
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

			inline std::shared_ptr<Bit_definition> new_bit_definiition(U8 scope_filter = ALL_SCOPES){
				current_bit_definition = get_current_block()->get_next_bit_def(scope_filter);
				return current_bit_definition;
			}
			
			std::shared_ptr<Variable> get_current_bit_definition_name();

			std::shared_ptr<Integer> get_current_bit_definition_size();

			inline std::shared_ptr<Gate> new_gate(const std::string& str, int num_qubits, int num_bits, int num_params, U64 hash = 0ULL){
				current_gate = std::make_shared<Gate>(str, hash, num_qubits, num_bits, num_params);

				current_qubit_op->set_gate_node(current_gate);

				return current_gate;
			}

			int get_current_gate_num_params();

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

			std::shared_ptr<Node> get_control_flow_stmt(const std::string& str, const U64& hash);

			std::shared_ptr<Compound_stmt> get_compound_stmt();

			std::shared_ptr<Compound_stmts> get_compound_stmts(std::shared_ptr<Node> parent);

			std::shared_ptr<Subroutines> new_subroutines_node();

			std::shared_ptr<Qubit_op> new_qubit_op_node(){
				reset(QUBIT_OP);

				current_qubit_op = std::make_shared<Qubit_op>(get_current_block());

				return current_qubit_op;
			}

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
	        unsigned int control_flow_depth;
		
			std::shared_ptr<Qubit_definition> current_qubit_definition;
			std::shared_ptr<Bit_definition> current_bit_definition;
			std::shared_ptr<Resource::Qubit> current_qubit;
			std::shared_ptr<Resource::Bit> current_bit;
			std::shared_ptr<Gate> current_gate;
			std::shared_ptr<Qubit_op> current_qubit_op;
			std::shared_ptr<Arg> current_arg;

			std::optional<std::shared_ptr<Subroutines>> subroutines_node = std::nullopt;
			std::optional<Genome> genome;
    };

}

#endif
