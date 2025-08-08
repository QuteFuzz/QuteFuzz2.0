#include <block.h>

size_t Block::make_register_resource_definition(int max_size, bool external, bool is_qubit, bool owned){

    size_t size;

    if(max_size > 1) size = random_int(max_size, 1);
    else size = max_size;

    if (is_qubit) {
        Register_resource_definition def(
            Variable("qreg" + std::to_string(qubit_defs.get_total())),
            Integer(std::to_string(size)),
            is_qubit,
            owned
        );

        def.make_resources(qubits, external, is_qubit, owned);

        qubit_defs.add(Resource_definition(def, external, is_qubit, owned));
    } else {
        Register_resource_definition def(
            Variable("creg" + std::to_string(bit_defs.get_total())),
            Integer(std::to_string(size)),
            is_qubit,
            owned
        );

        def.make_resources(bits, external, is_qubit, owned);

        bit_defs.add(Resource_definition(def, external, is_qubit, owned));
    }

    return size;
}

size_t Block::make_singular_resource_definition(bool external, bool is_qubit, bool owned){
    if (is_qubit) {
        Singular_resource_definition def (
            Variable("qubit" + std::to_string(qubit_defs.get_total())),
            is_qubit,
            owned
        );

        def.make_resources(qubits, external, is_qubit, owned);

        qubit_defs.add(Resource_definition(def, external, is_qubit, owned));
    } else {
        Singular_resource_definition def (
            Variable("bit" + std::to_string(bit_defs.get_total())),
            is_qubit,
            owned
        );

        def.make_resources(bits, external, is_qubit, owned);

        bit_defs.add(Resource_definition(def, external, is_qubit, owned));
    }    

    return 1;
}

size_t Block::make_resource_definitions(bool external, bool is_qubit, bool owned){
    int type_choice = random_int(1);

    #ifdef DEBUG
    INFO("Creating resource definitions");
    #endif
    
    int target_num_qubits = external ? target_num_qubits_external : target_num_qubits_internal;
    int target_num_bits = external ? target_num_bits_external : target_num_bits_internal;

    int target = is_qubit ? target_num_qubits : target_num_bits;

    while(target > 0){
        /*
            Use singular qubit or qubit register
        */
        if(type_choice){
            target -= make_singular_resource_definition(external, is_qubit, owned);

        } else {
            target -= make_register_resource_definition(target, external, is_qubit, owned);
        }

        type_choice = random_int(1);
    }

    return (external ? is_qubit ? qubit_defs.get_num_external() : bit_defs.get_num_external() : is_qubit ? qubit_defs.get_num_internal() : bit_defs.get_num_internal());
}

/// @brief Pick random qubit from combination of external and internal qubits
/// @return 
Resource::Resource* Block::get_random_resource(bool owned_only, bool is_qubit){
    size_t total_qubits = qubits.get_total();
    size_t total_bits = bits.get_total();

    if((is_qubit && total_qubits) || (!is_qubit && total_bits)){

        #ifdef DEBUG
        INFO("Getting random resource");
        #endif

        Resource::Resource* resource = is_qubit ? qubits.at(random_int(total_qubits - 1)) : bits.at(random_int(total_bits - 1));

        while(resource->is_used() || (owned_only && !resource->is_owned())){
            resource = is_qubit ? qubits.at(random_int(total_qubits - 1)) : bits.at(random_int(total_bits - 1));
        }

        resource->set_used();

        return resource;
    
    } else {
        return is_qubit ? &dummy_qubit : &dummy_bit;
    }
}

std::shared_ptr<Resource_definition> Block::get_next_resource_def(bool is_qubit){
    auto maybe_def = is_qubit ? qubit_defs.at(qubit_def_pointer++) : bit_defs.at(bit_def_pointer++);

    if(maybe_def == nullptr){
        return std::make_shared<Resource_definition>(is_qubit ? dummy_def : dummy_bit_def);

    } else {
        return std::make_shared<Resource_definition>(*maybe_def);

    }
}

std::shared_ptr<Resource_definition> Block::get_next_owned_resource_def(bool is_qubit){
    // Keep iterating maybe_def until we reach the end or find an owned/internal resource def
    auto maybe_def = is_qubit ? qubit_defs.at(qubit_def_pointer) : bit_defs.at(bit_def_pointer);
    while(maybe_def != nullptr && !maybe_def->is_owned() && (is_qubit ? qubit_def_pointer < qubit_defs.get_total() : bit_def_pointer < bit_defs.get_total())){
        maybe_def = is_qubit ? qubit_defs.at(++qubit_def_pointer) : bit_defs.at(++bit_def_pointer);
    }

    qubit_def_pointer++;

    if(maybe_def == nullptr){
        return std::make_shared<Resource_definition>(is_qubit ? dummy_def : dummy_bit_def);

    } else {

        return std::make_shared<Resource_definition>(*maybe_def);

    }
}

std::shared_ptr<Resource_definition> Block::get_next_external_resource_def(bool is_qubit){
    // Keep iterating maybe_def until we reach the end or find an external resource def
    auto maybe_def = is_qubit ? qubit_defs.at(qubit_def_pointer) : bit_defs.at(bit_def_pointer);
    while(maybe_def != nullptr && !maybe_def->is_external() && (is_qubit ? qubit_def_pointer < qubit_defs.get_total() : bit_def_pointer < bit_defs.get_total())){
        maybe_def = is_qubit ? qubit_defs.at(++qubit_def_pointer) : bit_defs.at(++bit_def_pointer);
    }

    qubit_def_pointer++;

    if(maybe_def == nullptr){
        return std::make_shared<Resource_definition>(is_qubit ? dummy_def : dummy_bit_def);

    } else {

        return std::make_shared<Resource_definition>(*maybe_def);

    }
}
