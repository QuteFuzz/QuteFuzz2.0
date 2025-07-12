#include <block.h>

size_t Block::make_register_qubit_definition(int max_size, bool external){

    size_t size;

    if(max_size > 1) size = random_int(max_size, 1);
    else size = max_size;
    
    if(external) {
        Register_qubit_definition def(
            Variable("qreg" + std::to_string(register_qubit_def_count++)),
            Integer(std::to_string(size))    
        );
        def.make_qubits(external_qubits);
        qubit_defs.push_back(Qubit_definition::Qubit_definition(def));
    } 
    else {
        Register_qubit_definition_int def(
            Variable("qreg" + std::to_string(register_qubit_def_count++)),
            Integer(std::to_string(size))    
        );
        def.make_qubits(internal_qubits);
        qubit_defs.push_back(Qubit_definition::Qubit_definition(def));
    }
    
    return size;
}


size_t Block::make_singular_qubit_definition(bool external){

    if(external) {
        Singular_qubit_definition def (
            Variable("qubit" + std::to_string(singular_qubit_def_count++))
        );
        def.make_qubits(external_qubits);
        qubit_defs.push_back(Qubit_definition::Qubit_definition(def));
    }
    else {
        Singular_qubit_definition_int def(
            Variable("qubit" + std::to_string(singular_qubit_def_count++))
        );
        def.make_qubits(internal_qubits);
        qubit_defs.push_back(Qubit_definition::Qubit_definition(def));
    }
    
    return 1;
}


void Block::make_qubit_definitions(bool external){
    int type_choice = 0; // random_int(1);

    #ifdef DEBUG
    INFO("Creating qubit definition");
    #endif

    while(external ? target_num_qubits : target_num_qubits_int > 0){
        /*
            Use singular qubit or qubit register
        */
        if(type_choice){
            if(external) target_num_qubits -= make_singular_qubit_definition(external);
            else target_num_qubits_int -= make_singular_qubit_definition(external);
        } else {
            if(external) target_num_qubits -= make_register_qubit_definition(target_num_qubits, external);
            else target_num_qubits_int -= make_register_qubit_definition(target_num_qubits_int, external);
        }

        // type_choice = random_int(1);
    }
}

Qubit::Qubit* Block::get_qubit_at(size_t index){
    if(index < num_external_qubits()) return &external_qubits.at(index);
    else return &dummy_qubit;
}

/// @brief Pick random qubit from combination of external and internal qubits
/// @param best_entanglement 
/// @return 
std::shared_ptr<Qubit::Qubit> Block::get_random_qubit(std::optional<std::vector<int>> best_entanglement){

    if(num_external_qubits()){

        Qubit::Qubit* qubit = get_qubit_at(0);

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
            size_t total_qubits = num_external_qubits();

            qubit = get_qubit_at(random_int(total_qubits - 1));
            
            while(qubit->is_used()){
                qubit = get_qubit_at(random_int(total_qubits - 1));
            }

            qubit->set_used();
        }

        return std::make_shared<Qubit::Qubit>(*qubit);
    
    } else {
        return std::make_shared<Qubit::Qubit>(dummy_qubit);
    }
}