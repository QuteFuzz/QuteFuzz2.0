#include <block.h>

size_t Block::make_register_qubit_definition(int max_size, bool external){

    size_t size;

    if(max_size > 1) size = random_int(max_size, 1);
    else size = max_size;

    Register_qubit_definition def(
        Variable("qreg" + std::to_string(qubit_defs.get_total())),
        Integer(std::to_string(size))    
    );

    def.make_qubits(qubits, external);
    
    qubit_defs.add(Qubit_definition::Qubit_definition(def, external));
    
    return size;
}


size_t Block::make_singular_qubit_definition(bool external){
    Singular_qubit_definition def (
        Variable("qubit" + std::to_string(qubit_defs.get_total()))
    );

    def.make_qubits(qubits, external);

    qubit_defs.add(Qubit_definition::Qubit_definition(def, external));

    return 1;
}


size_t Block::make_qubit_definitions(bool external){
    int type_choice = random_int(1);

    #ifdef DEBUG
    INFO("Creating qubit definitions");
    #endif

    int target_num_qubits = external ? target_num_qubits_external : target_num_qubits_internal;

    while(target_num_qubits > 0){
        /*
            Use singular qubit or qubit register
        */
        if(type_choice){
            target_num_qubits -= make_singular_qubit_definition(external);

        } else {
            target_num_qubits -= make_register_qubit_definition(target_num_qubits, external);
        }

        type_choice = random_int(1);
    }

    return (external ? qubit_defs.get_num_external() : qubit_defs.get_num_internal());
}

/// @brief Pick random qubit from combination of external and internal qubits
/// @return 
Qubit::Qubit* Block::get_random_qubit(){
    size_t total_qubits = qubits.get_total();

    if(total_qubits){
        
        #ifdef DEBUG
        INFO("Getting random qubit");
        #endif

        Qubit::Qubit* qubit = qubits.at(random_int(total_qubits - 1));
        
        while(qubit->is_used()){
            qubit = qubits.at(random_int(total_qubits - 1));
        }

        qubit->set_used();

        return qubit;
    
    } else {
        return &dummy_qubit;
    }
}

std::shared_ptr<Qubit_definition::Qubit_definition> Block::get_next_qubit_def(){
    auto maybe_def = qubit_defs.at(qubit_def_pointer++);

    if(maybe_def == nullptr){
        return std::make_shared<Qubit_definition::Qubit_definition>(dummy_def);
            
    } else {
        return std::make_shared<Qubit_definition::Qubit_definition>(*maybe_def); 

    }
}

std::shared_ptr<Qubit_definition::Qubit_definition> Block::get_next_owned_qubit_def(){
    // Keep iterating maybe_def until we reach the end or find an owned/internal qubit def
    auto maybe_def = qubit_defs.at(qubit_def_pointer);
    while(maybe_def != nullptr && maybe_def->is_external()){
        maybe_def = qubit_defs.at(++qubit_def_pointer);
    }

    qubit_def_pointer++;

    if(maybe_def == nullptr){
        return std::make_shared<Qubit_definition::Qubit_definition>(dummy_def);
            
    } else {
        
        return std::make_shared<Qubit_definition::Qubit_definition>(*maybe_def); 

    }
}

std::shared_ptr<Qubit_definition::Qubit_definition> Block::get_next_external_qubit_def(){
    // Keep iterating maybe_def until we reach the end or find an external qubit def
    auto maybe_def = qubit_defs.at(qubit_def_pointer);
    while(maybe_def != nullptr && !maybe_def->is_external()){
        maybe_def = qubit_defs.at(++qubit_def_pointer);
    }

    qubit_def_pointer++;

    if(maybe_def == nullptr){
        return std::make_shared<Qubit_definition::Qubit_definition>(dummy_def);
            
    } else {
        
        return std::make_shared<Qubit_definition::Qubit_definition>(*maybe_def); 

    }
}
