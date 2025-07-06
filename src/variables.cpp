#include "../include/variables.h"

int Qreg::count = 0;

size_t Variables::setup_qregs(int num_qubits){

    while(num_qubits > 0){
        size_t qreg_size;

        if(num_qubits > 1) qreg_size = random_int(num_qubits, 1);
        else qreg_size = num_qubits;
        
        push_back(Qreg(qreg_size));

        num_qubits -= qreg_size;
    }

    return num_qregs();
}


size_t Variables::setup_qubits(int num_qubits){
    push_back(Qreg(num_qubits, "qubit"));
    return num_qubits;
}

/// @brief Pick random qubit from combination of external and internal qubits
/// @param best_entanglement 
/// @return 
std::shared_ptr<Qubit> Variables::get_random_qubit(std::optional<std::vector<int>> best_entanglement){

    std::shared_ptr<Qubit> qubit = DEFAULT_QUBIT;

    if(best_entanglement.has_value()){
        std::vector<int> e = best_entanglement.value();

        qubit = get_qubit_at(e[0]);
        int pointer = 0;

        while(qubit->is_used()){
            qubit = get_qubit_at(e[++pointer]);
            std::cout << *qubit << std::endl;
        }

        qubit->set_used();

    } else {

        size_t total_qubits = total_num_qubits();

        if(total_qubits){
            qubit = get_qubit_at(random_int(total_qubits - 1));
            
            while(qubit->is_used()){
                qubit = get_qubit_at(random_int(total_qubits - 1));
            }

            qubit->set_used();        
        
        }
    }

    return qubit;
}


std::shared_ptr<Qubit> Variables::get_random_singular_qubit(){

    size_t total_qubits = total_num_qubits();

    std::shared_ptr<Qubit> qubit = DEFAULT_QUBIT;

    if(total_qubits){
        int index = random_int(total_qubits - 1);
        qubit = get_qubit_at(index);
        
        while(qubit->is_used() || qubit->is_from_register()){
            index = random_int(total_qubits - 1);
            qubit = get_qubit_at(index);
        }

        qubit->set_used();
    
    }

    return qubit;
}


std::shared_ptr<Qubit> Variables::get_qubit_at(size_t index){
    assert(index < total_num_qubits());

    size_t externals_size = external_qubits.size();

    if(index < externals_size){
        return external_qubits[index];
    } else {
        return internal_qubits[index - externals_size];
    }
}
