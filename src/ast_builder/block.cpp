#include "../../include/ast_builder/block.h"

size_t Block::add_qubit(bool external){
    std::shared_ptr<Qubit_def::Qubit> qubit = std::make_shared<Qubit_def::Qubit>("qubit");

    if(external) {
        external_qubits.push_back(qubit);
        
        // if adding externally, this qubit is also definition
        qubit_defs.push_back(std::make_shared<Qubit_def::Qubit_def>(qubit));

    } else {
        internal_qubits.push_back(qubit);
    }

    return 1;
}

size_t Block::add_qreg(int max_num_qubits, bool external){

    size_t qreg_size;

    if(max_num_qubits > 1) qreg_size = random_int(max_num_qubits, 1);
    else qreg_size = max_num_qubits;

    std::shared_ptr<Qubit_def::Qreg> qreg = std::make_shared<Qubit_def::Qreg>(qreg_size);
    
    if(external){
        qreg->make_qubits(external_qubits);

        // if adding externally, this qreg is also a definition
        qubit_defs.push_back(std::make_shared<Qubit_def::Qubit_def>(qreg));

    } else {
        qreg->make_qubits(internal_qubits);
        
        qubit_defs.push_back(std::make_shared<Qubit_def::Qubit_def>(qreg));
    }

    return qreg_size;
}

/// @brief Qubit_def::Qubit definitions used by this block (Always picks qubit register definitions)
/// TODO: Properly add support for single qubit definitions with needed checks for safety
/// @param num_qubits is the total number of qubits a suboutine should have (internal and external)
/// @return `num_qubit_defs`
size_t Block::setup_qubit_defs(int num_qubits, bool external){
    int type_choice = 0; // random_int(1);
    size_t num_qubit_defs = 0;

    #ifdef DEBUG
    INFO("Creating qubit definition");
    #endif

    while(num_qubits > 0){
        /*
            Use singular qubit or qubit register
        */
        if(type_choice){
            num_qubits -= add_qubit(external);
        } else {
            num_qubits -= add_qreg(num_qubits, external);
        }
        
        // type_choice = random_int(1);
        num_qubit_defs++;
        

    }

    return num_qubit_defs;
}

/// @brief Pick random qubit from combination of external and internal qubits
/// @param best_entanglement 
/// @return 
std::shared_ptr<Qubit_def::Qubit> Block::get_random_qubit(std::optional<std::vector<int>> best_entanglement){

    std::shared_ptr<Qubit_def::Qubit> qubit = Qubit_def::DEFAULT_QUBIT;

    #ifdef DEBUG
    INFO("Getting random qubit");
    #endif

    if(best_entanglement.has_value()){
        std::vector<int> e = best_entanglement.value();

        qubit = get_qubit_at(e[0]);
        int pointer = 0;

        while(qubit->is_used()){
            qubit = get_qubit_at(e[++pointer]);
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


std::shared_ptr<Qubit_def::Qubit> Block::get_qubit_at(size_t index){
    assert(index < total_num_qubits());

    size_t externals_size = external_qubits.size();

    if(index < externals_size){
        return external_qubits[index];
    } else {
        return internal_qubits[index - externals_size];
    }
}
