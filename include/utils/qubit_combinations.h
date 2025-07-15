#ifndef QUBIT_COMBS_H
#define QUBIT_COMBS_H

#include <utils.h>

struct Qubit_combinations{
    
    public:
        Qubit_combinations(){
            set_possible_qubit_combinations();
        }

        void set(int num_qubits, int num_qubits_in_entanglement, std::vector<std::vector<int>> entanglements){
            data[num_qubits-1][num_qubits_in_entanglement-1] = std::move(entanglements);
        }

        std::vector<std::vector<int>> at(int num_qubits, int num_qubits_in_entanglement) const {
            return data[num_qubits-1][num_qubits_in_entanglement-1];
        }

        void set_possible_qubit_combinations(){

            for(int n_qubits = Common::MIN_N_QUBITS_IN_ENTANGLEMENT; n_qubits <= Common::MAX_QUBITS*2; n_qubits++){
                for(int n_qubits_in_entanglement = Common::MIN_N_QUBITS_IN_ENTANGLEMENT; n_qubits_in_entanglement <= n_qubits; n_qubits_in_entanglement++){
                    set(n_qubits, n_qubits_in_entanglement, n_choose_r(n_qubits, n_qubits_in_entanglement));
                }
            }
            
        }

        friend std::ostream& operator<<(std::ostream& stream, Qubit_combinations& combs){

            for(int n_qubits = Common::MIN_N_QUBITS_IN_ENTANGLEMENT; n_qubits <= Common::MAX_QUBITS; n_qubits++){

                stream << "N qubits: " << n_qubits << std::endl;
                stream << "==================================" << std::endl;

                for(int n_qubits_in_entanglement = Common::MIN_N_QUBITS_IN_ENTANGLEMENT; n_qubits_in_entanglement <= n_qubits; n_qubits_in_entanglement++){
                    
                    stream << "n_qubits_in_entanglement: " << n_qubits_in_entanglement << std::endl;

                    for(const std::vector<int>& entanglement : combs.at(n_qubits, n_qubits_in_entanglement)){
                        for(const int& i : entanglement) std::cout << i << " ";
                        stream << " ";
                    }
                    stream << std::endl;
                }
            }

            return stream;
        }

    private:
        std::vector<std::vector<int>> data[Common::MAX_QUBITS*2][Common::MAX_QUBITS*2];
};

#endif
