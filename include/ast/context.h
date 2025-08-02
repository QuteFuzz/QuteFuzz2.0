#ifndef CONTEXT_H
#define CONTEXT_H

#include <block.h>
#include <qubit_definition.h>
#include <variable.h>
#include <qubit_defs.h>
#include <discard_qubit_defs.h>
#include <discard_qubit_def.h>
#include <arg.h>
#include <compound_stmt.h>
#include <gate.h>


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

			std::string get_current_block_owner(){
				return current_block_owner;
			}

            void set_can_apply_subroutines(bool flag = true);

            size_t get_max_defined_qubits();

			std::shared_ptr<Block> get_current_block() const;

			std::shared_ptr<Block> get_random_block();

            std::shared_ptr<Block> setup_block(std::string str, U64 hash);
            
			std::shared_ptr<Qubit_defs> make_qubit_definitions(std::string& str, U64& hash);

			std::optional<std::shared_ptr<Block>> get_block(std::string owner);

			void set_current_qubit();

			std::shared_ptr<Qubit::Qubit> get_current_qubit();

			void set_current_arg(const std::string& str, const U64& hash);

			std::shared_ptr<Arg> get_current_arg();

			std::shared_ptr<Integer> get_current_qubit_index();

			std::shared_ptr<Variable> get_current_qubit_name();

			void set_current_qubit_definition();

			std::shared_ptr<Qubit_definition::Qubit_definition> get_current_qubit_definition();

			void set_current_qubit_definition_owned();

			std::shared_ptr<Discard_qubit_def> get_current_qubit_definition_discard(const std::string& str, const U64& hash);

			std::shared_ptr<Integer> get_current_qubit_definition_size();

			std::shared_ptr<Variable> get_current_qubit_definition_name();

			void set_current_gate(const std::string& str, int num_qubits, int num_params);

			std::shared_ptr<Gate> get_current_gate();

			std::shared_ptr<Discard_qubit_defs> get_discard_qubit_defs(const std::string& str, const U64& hash, int num_owned_qubit_defs);

			std::shared_ptr<Node> get_control_flow_stmt(const std::string& str, const U64& hash);

			std::shared_ptr<Compound_stmt> get_compound_stmt(const std::string& str, const U64& hash);

			int get_current_gate_num_params();

			int get_current_gate_num_qubits();
		
			bool current_block_is_subroutine(){
                return (subroutines_node != nullptr) && (subroutines_node->build_state() == NB_BUILD);
            }

			void set_subroutines_node(std::shared_ptr<Node> _node){
				subroutines_node = _node;
			}

			void set_current_gate_definition();

			std::shared_ptr<Block> get_current_gate_definition() const {
				return current_gate_definition.value_or(std::make_shared<Block>(dummy_block));
			}

        private:
			std::string current_block_owner;
            std::vector<std::shared_ptr<Block>> blocks;
			
			Block dummy_block;
			Integer dummy_int;
			Variable dummy_var;

            int subroutine_counter = 0;
			
			std::shared_ptr<Qubit_definition::Qubit_definition> current_qubit_definition;
			std::shared_ptr<Qubit::Qubit> current_qubit;
			std::shared_ptr<Gate> current_gate;
			size_t current_port = 0;

			std::shared_ptr<Arg> current_arg;

			std::shared_ptr<Node> subroutines_node = nullptr;
			
			std::optional<std::shared_ptr<Block>> current_gate_definition = std::nullopt; // not all gates have definitions

	        size_t compound_stmt_depth = Common::COMPOUND_STMT_DEPTH;
    };

}

#endif
