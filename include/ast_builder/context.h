#ifndef CONTEXT_H
#define CONTEXT_H

#include <block.h>
#include <qubit_definition.h>
#include <graph.h>
#include <variable.h>

class Gate;

namespace Context {

    enum Level {
        PROGRAM,
        QUBIT_OP,
    };

    struct Context {

        public:
            Context(){}

			void reset(Level l);

            void set_circuits_dir(fs::path dir){
                circuit_dir = dir;
            }

			std::string get_current_block_owner(){
				return current_block_owner;
			}

            void set_can_apply_subroutines();

            size_t get_max_defined_qubits();

			std::shared_ptr<Block> get_current_block() const;

			std::shared_ptr<Block> get_random_block();

            std::shared_ptr<Block> setup_block(std::string str, U64 hash);
            
			size_t make_qubit_definitions(bool external = true);

			std::shared_ptr<Qubit::Qubit> get_current_qubit();

			std::shared_ptr<Integer> get_current_qubit_index();

			std::shared_ptr<Variable> get_current_qubit_name();

			std::shared_ptr<Qubit_definition::Qubit_definition> get_current_qubit_definition();

			std::shared_ptr<Integer> get_current_qubit_definition_size();

			std::shared_ptr<Variable> get_current_qubit_definition_name();

			std::shared_ptr<Gate> get_current_gate(std::string str, int num_qubits, int num_params);

			int get_current_gate_num_params();

			int get_current_gate_num_qubits();
		
			bool current_block_is_subroutine(){
                return (subroutines_node != nullptr) && (subroutines_node->build_state() == NB_BUILD);
            }

			void set_subroutines_node(std::shared_ptr<Node> _node){
				subroutines_node = _node;
			}

			void set_qig();

        private:
			std::string current_block_owner;
            std::vector<std::shared_ptr<Block>> blocks;

            int subroutine_counter = 0;
            
            std::shared_ptr<Graph> qig = nullptr;
            fs::path circuit_dir;
			
			std::shared_ptr<Qubit_definition::Qubit_definition> current_qubit_definition;
			std::shared_ptr<Qubit::Qubit> current_qubit;
			std::shared_ptr<Gate> current_gate;
			std::shared_ptr<Node> subroutines_node = nullptr;
    };

}

#endif
