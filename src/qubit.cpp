#include "../include/qubit.h"

int Qreg::count = 0;

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

std::shared_ptr<Qubit> Qreg_definitions::get_random_qubit(std::optional<std::pair<int, int>> pair){

    if(pair.has_value()){
        std::pair<int, int> p = pair.value();
        Qubit* qubit = &qubits[p.first];

        if(qubit->is_used()){
            qubit = &qubits[p.second];
        }

        qubit->set_used();
        return std::make_shared<Qubit>(*qubit);

    } else {

        if(qubits.size()){
            int index = random_int(qubits.size() - 1);
            Qubit* qubit = &qubits[index];
            
            while(qubit->is_used()){
                index = random_int(qubits.size() - 1);
                qubit = &qubits[index];
            }

            qubit->set_used();
        
            return std::make_shared<Qubit>(qubits[index]);
        
        } else {
            return DEFAULT_QUBIT;
        }
    }
}