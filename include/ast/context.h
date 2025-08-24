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
		COMPOUND_STMT,
    };

    struct Context {

        public:
            Context(){}

			void reset(Level l);

			std::string get_current_block_owner(){
				return current_block_owner;
			}

            void set_can_apply_subroutines(bool flag = true);

            unsigned int get_max_external_qubits();

			unsigned int get_max_external_bits();

			std::shared_ptr<Block> get_current_block() const;

			std::shared_ptr<Block> get_random_block();

            std::shared_ptr<Block> setup_block(std::string str, U64 hash);
            
			std::shared_ptr<Qubit_defs> make_qubit_definitions(std::string& str, U64& hash);

			std::shared_ptr<Bit_defs> make_bit_definitions(std::string& str, U64& hash);

			std::optional<std::shared_ptr<Block>> get_block(std::string owner);

			void set_current_qubit();

			void set_current_bit();

			std::shared_ptr<Resource::Qubit> get_current_qubit();

			std::shared_ptr<Resource::Bit> get_current_bit();

			inline std::shared_ptr<Resource::Qubit> new_qubit(){
				set_current_qubit();
				return get_current_qubit();
			}

			inline std::shared_ptr<Resource::Bit> new_bit(){
				set_current_bit();
				return get_current_bit();
			}
		
			void set_current_arg();

			std::shared_ptr<Arg> get_current_arg();

			inline std::shared_ptr<Arg> new_arg(){
				set_current_arg();
				return get_current_arg();
			}

			std::shared_ptr<Integer> get_current_qubit_index();

			std::shared_ptr<Integer> get_current_bit_index();

			std::shared_ptr<Variable> get_current_qubit_name();

			std::shared_ptr<Variable> get_current_bit_name();

			void set_current_qubit_definition();

			void set_current_bit_definition();

			std::shared_ptr<Qubit_definition> get_current_qubit_definition();

			std::shared_ptr<Bit_definition> get_current_bit_definition();

			inline std::shared_ptr<Qubit_definition> new_qubit_definition(){
				set_current_qubit_definition();
				return get_current_qubit_definition();
			}

			inline std::shared_ptr<Bit_definition> new_bit_definiition(){
				set_current_bit_definition();
				return get_current_bit_definition();
			}

			void set_current_qubit_definition_owned();

			void set_current_bit_definition_owned();

			std::shared_ptr<Node> get_current_qubit_definition_discard(const std::string& str, const U64& hash);

			std::shared_ptr<Integer> get_current_qubit_definition_size();

			std::shared_ptr<Variable> get_current_qubit_definition_name();

			std::shared_ptr<Integer> get_current_qubit_definition_size_including_single();

			std::shared_ptr<Integer> get_current_bit_definition_size();

			std::shared_ptr<Variable> get_current_bit_definition_name();

			void set_current_gate(const std::string& str, int num_qubits, int num_bits, int num_params, U64 hash = 0ULL);

			std::shared_ptr<Gate> get_current_gate();

			inline std::shared_ptr<Gate> new_gate(const std::string& str, int num_qubits, int num_bits, int num_params, U64 hash = 0ULL){
				set_current_gate(str, num_qubits, num_bits, num_params, hash);
				return get_current_gate();
			}

			/// @brief Make barrier gate. The number of qubits used to choose the barrier width is a max of internal and external qubits, 
			/// because a block could have either or both
			/// @return 
			inline std::shared_ptr<Gate> get_barrier(){
				std::shared_ptr<Block> current_block = get_current_block();

				unsigned int n_qubits = std::min((unsigned int)WILDCARD_MAX, (unsigned int)current_block->total_num_qubits());
				unsigned int random_barrier_width = random_int(n_qubits, 1);

				return new_gate("barrier", random_barrier_width, 0, 0);
			}

			std::shared_ptr<Node> get_discard_qubit_defs(const std::string& str, const U64& hash, int num_owned_qubit_defs);

			std::shared_ptr<Node> get_control_flow_stmt(const std::string& str, const U64& hash);

			void set_current_compound_stmt();

			std::shared_ptr<Compound_stmt> get_current_compound_stmt();

			inline std::shared_ptr<Compound_stmt> new_compound_stmt(){
				set_current_compound_stmt();
				return get_current_compound_stmt();
			} 

			std::shared_ptr<Compound_stmts> get_compound_stmts(std::shared_ptr<Node> parent);

			std::shared_ptr<Compound_stmts> get_compound_stmts(std::shared_ptr<Node> parent);

			int get_current_gate_num_params();

			int get_current_gate_num_qubits();

			int get_current_gate_num_bits();
			
			/// @brief Is the current block being generated a subroutine?
			/// @return 
			inline bool current_block_is_subroutine(){
                return (subroutines_node != nullptr) && (subroutines_node->build_state() == NB_BUILD);
            }

			std::shared_ptr<Subroutines> get_subroutines_node();

			void set_current_gate_definition();

			inline std::shared_ptr<Block> get_current_gate_definition() const {
				return current_gate_definition.value_or(std::make_shared<Block>(dummy_block));
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

            int subroutine_counter = 0;
			int ast_counter = 0;
			
			std::shared_ptr<Qubit_definition> current_qubit_definition;
			std::shared_ptr<Bit_definition> current_bit_definition;
			std::shared_ptr<Resource::Qubit> current_qubit;
			std::shared_ptr<Resource::Bit> current_bit;
			std::shared_ptr<Gate> current_gate;
			std::shared_ptr<Compound_stmt> current_compound_stmt;
			size_t current_port = 0;

			std::shared_ptr<Arg> current_arg;

			std::shared_ptr<Subroutines> subroutines_node = nullptr;
			
			std::optional<std::shared_ptr<Block>> current_gate_definition = std::nullopt; // not all gates have definitions

	        size_t compound_stmt_depth = Common::COMPOUND_STMT_DEPTH;

			std::optional<Genome> genome;
    };

}

#endif
