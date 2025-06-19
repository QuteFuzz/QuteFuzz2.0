#include "../include/qubit.h"

int Qreg::count = 0;

std::unordered_map<int, std::vector<std::pair<int, int>>> N_QUBIT_TO_PAIRINGS_MAP;

void set_n_qubit_to_pairings_map(){
    for(int i = 2; i < Common::MAX_QUBITS; i++){
        N_QUBIT_TO_PAIRINGS_MAP[i] = n_choose_2(i);
    }
}

/// @brief Create qregs and qubit definitions
/// @param qreg_defs 
size_t Qreg_definitions::setup_qregs(int minimum_num_qubits){

    Qreg::count = 0;
    int num_qubits = random_int(Common::MAX_QUBITS, minimum_num_qubits);

    while(num_qubits > 0){
        size_t qreg_size;

        if(num_qubits > 1) qreg_size = random_int(num_qubits, 1);
        else qreg_size = num_qubits;
        
        push_back(Qreg(qreg_size));

        num_qubits -= qreg_size;
    }

    // std::cout << qreg_defs << std::endl;

    return num_qregs();
}