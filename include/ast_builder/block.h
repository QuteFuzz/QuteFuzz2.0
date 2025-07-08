#ifndef BLOCK_H
#define BLOCK_H

#include "qubit_def.h"

class Block {

    public:

        Block(std::string _circuit) : circuit(_circuit) {}

        inline std::string owner(){
            return circuit;
        }

        /// @brief Number of qubits defined by this block. Counts external qubits by default 
        /// @param external_qubits 
        /// @return 
        inline size_t num_qubits(bool external = true){
            return (external ? external_qubits.size() : internal_qubits.size());
        }

        inline size_t total_num_qubits(){
            return external_qubits.size() + internal_qubits.size();
        }

        /// @brief Reset all stores and pointers
        void reset(){
            external_qubits.clear();
            internal_qubits.clear();
        }

        /// @brief Reset used flag for each qubit so that it can be chosen for this gate or subroutine
        void qubit_flag_reset(){
            for(auto& qb : external_qubits){
                qb->reset();
            }

            for(auto& qb : internal_qubits){
                qb->reset();
            }
        }
    
        inline std::shared_ptr<Qubit_def::Qubit_def> get_next_qubit_def(){
            return qubit_defs[qubit_def_pointer++];
        }

        std::shared_ptr<Qubit_def::Qubit> get_random_qubit(std::optional<std::vector<int>> best_entanglement);

        std::shared_ptr<Qubit_def::Qubit> get_qubit_at(size_t index);

        friend std::ostream& operator<<(std::ostream& stream, Block block){
            stream << "Owner " << block.circuit << std::endl; 
            stream << "=====================" << std::endl;
            stream << "Qubit definitions, size : " << block.qubit_defs.size() << std::endl;
            for(const auto& qubit_def : block.qubit_defs){
                stream << *qubit_def << std::endl;
            }

            stream << "=====================" << std::endl;
            stream << "External Qubits, size: " << block.external_qubits.size() << std::endl;
            for(const auto& qb : block.external_qubits){
                stream << *qb << std::endl;
            }

            stream << "=====================" << std::endl;
            stream << "Internal Qubits, size: " << block.internal_qubits.size() << std::endl;
            for(const auto& qb : block.internal_qubits){
                stream << *qb << std::endl;
            }

            return stream;
        }
        
        /// @brief Are these qreg definitions owned by this circuit?
        /// @param circuit_name 
        /// @return 
        bool owned_by(std::string circuit_name){
            return circuit == circuit_name;
        }

        size_t add_qreg(int max_num_qubits, bool external);

        size_t add_qubit(bool external);

        size_t setup_qubit_defs(int num_qubits, bool external = true);

    private:
        std::string circuit;

        std::vector<std::shared_ptr<Qubit_def::Qubit>> external_qubits;
        std::vector<std::shared_ptr<Qubit_def::Qubit>> internal_qubits;

        std::vector<std::shared_ptr<Qubit_def::Qubit_def>> qubit_defs;
        int qubit_def_pointer = 0;
};


#endif
