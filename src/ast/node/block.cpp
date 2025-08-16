#include <block.h>

Resource::Qubit* Block::get_random_qubit(U8 scope_filter){
    size_t total_qubits = qubits.get_total();
    
    if(total_qubits){

        #ifdef DEBUG
        INFO("Getting random qubit");
        #endif

        Resource::Qubit* qubit = qubits.at(random_int(total_qubits - 1));

        while(qubit->is_used() || !(qubit->get_scope() & scope_filter)){
            qubit = qubits.at(random_int(total_qubits - 1));
        }

        qubit->set_used();

        return qubit;
    
    } else {
        return &dummy_qubit;
    }
}

Resource::Bit* Block::get_random_bit(U8 scope_filter){
    size_t total_bits = bits.get_total();
    
    if(total_bits){

        #ifdef DEBUG
        INFO("Getting random bit");
        #endif

        Resource::Bit* bit = bits.at(random_int(total_bits - 1));

        while(bit->is_used() || !(bit->get_scope() & scope_filter)){
            bit = bits.at(random_int(total_bits - 1));
        }

        bit->set_used();

        return bit;
    
    } else {
        return &dummy_bit;
    }
}


std::shared_ptr<Qubit_definition> Block::get_next_qubit_def(U8 scope_filter){
    Qubit_definition* maybe_def = qubit_defs.at(qubit_def_pointer++);

    while((maybe_def != nullptr) && !(maybe_def->get_scope() & scope_filter)){
        maybe_def = qubit_defs.at(qubit_def_pointer++);
    }

    if(maybe_def == nullptr){
        return std::make_shared<Qubit_definition>(dummy_qubit_def);
    } else {
        return std::make_shared<Qubit_definition>(*maybe_def);
    }
}

std::shared_ptr<Bit_definition> Block::get_next_bit_def(U8 scope_filter){
    Bit_definition* maybe_def = bit_defs.at(bit_def_pointer++);

    while((maybe_def != nullptr) && !(maybe_def->get_scope() & scope_filter)){
        maybe_def = bit_defs.at(bit_def_pointer++);
    }

    if(maybe_def == nullptr){
        return std::make_shared<Bit_definition>(dummy_bit_def);
    } else {
        return std::make_shared<Bit_definition>(*maybe_def);
    }
}

size_t Block::make_register_resource_definition(int max_size, U8 scope, Resource::Classification classification, size_t& total_definitions){

    size_t size;

    if(max_size > 1) size = random_int(max_size, 1);
    else size = max_size;

    if (classification == Resource::QUBIT) {
        Register_qubit_definition def(
            Variable("qreg" + std::to_string(qubit_defs.get_total())),
            Integer(std::to_string(size))
        );

        def.make_resources(qubits, scope);

        qubit_defs.add(Qubit_definition(def, scope));

    } else {
        Register_bit_definition def(
            Variable("creg" + std::to_string(bit_defs.get_total())),
            Integer(std::to_string(size))
        );

        def.make_resources(bits, scope);

        bit_defs.add(Bit_definition(def, scope));
    }

    total_definitions += 1;

    return size;
}

size_t Block::make_singular_resource_definition(U8 scope, Resource::Classification classification, size_t& total_definitions){
    if (classification == Resource::QUBIT) {
        Singular_qubit_definition def (
            Variable("qubit" + std::to_string(qubit_defs.get_total()))
        );

        def.make_resources(qubits, scope);

        qubit_defs.add(Qubit_definition(def, scope));

    } else {
        Singular_bit_definition def (
            Variable("bit" + std::to_string(bit_defs.get_total()))
        );

        def.make_resources(bits, scope);

        bit_defs.add(Bit_definition(def, scope));
    }

    total_definitions += 1;

    return 1;
}

size_t Block::make_resource_definitions(U8 scope, Resource::Classification classification){
    int type_choice = random_int(1);

    #ifdef DEBUG
    INFO("Creating resource definitions");
    #endif
    
    int target_num_qubits = (Resource::is_external(scope)) ? target_num_qubits_external : target_num_qubits_internal;
    int target_num_bits = (Resource::is_external(scope)) ? target_num_bits_external : target_num_bits_internal;

    int target = (classification == Resource::QUBIT) ? target_num_qubits : target_num_bits;

    size_t total_num_definitions = 0;

    while(target > 0){
        /*
            Use singular qubit or qubit register
        */
        if(type_choice){
            target -= make_singular_resource_definition(scope, classification, total_num_definitions);

        } else {
            target -= make_register_resource_definition(target, scope, classification, total_num_definitions);
        }

        type_choice = random_int(1);
    }

    return total_num_definitions;
}

/// @brief Given set of qubits from genome, create qubit definitions from those qubits
/// @param scope 
/// @param qubits 
/// @return 
size_t Block::make_resource_definitions(U8 scope, const Collection<Resource::Qubit>& _qubits){
    size_t num_definitions = 0;

    qubits = _qubits;

    // only create definitions from qubits matching the scope
    for(const Resource::Qubit& qubit : _qubits){
        if(qubit.get_scope() & scope){
            num_definitions += qubit_to_qubit_def(scope, qubit);
        }
    }
    
    return num_definitions;
}

/// @brief Add qubit to set of qubit defs in block, merge with register as required
/// @param qubit 
size_t Block::qubit_to_qubit_def(const U8& scope, const Resource::Qubit& qubit){
    for(Qubit_definition& qubit_def : qubit_defs){
        if(qubit_def.defines(qubit)){
            qubit_def.increase_size();
            return 0;
        }
    }

    if(qubit.is_register_def()){
        Register_qubit_definition qubit_def(qubit);
        qubit_defs.add(Qubit_definition(qubit_def, scope));

    } else {
        Singular_qubit_definition qubit_def(qubit);
        qubit_defs.add(Qubit_definition(qubit_def, scope));
    }

    return 1;
}
