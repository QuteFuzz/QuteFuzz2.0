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

/// @brief Make a register resource definition, whose size is bounded by `max_size`
/// @param max_size 
/// @param scope 
/// @param classification 
/// @param total_definitions 
/// @return number of resources created from this definition
unsigned int Block::make_register_resource_definition(unsigned int max_size, U8 scope, Resource::Classification classification, unsigned int& total_definitions){

    unsigned int size;

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

/// @brief Make singular resource definition 
/// @param scope 
/// @param classification 
/// @param total_definitions 
/// @return 1, since there's one qubit created from a singular resource definition
unsigned int Block::make_singular_resource_definition(U8 scope, Resource::Classification classification, unsigned int& total_definitions){
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

unsigned int Block::make_resource_definitions(U8 scope, Resource::Classification classification){

    unsigned int target_num_resources = 0, total_num_definitions = 0;

    bool scope_is_external = Resource::is_external(scope);
    bool classificaton_is_qubit = (classification == Resource::QUBIT);
    
    switch((scope_is_external << 1) | classificaton_is_qubit){
        case 0b00: target_num_resources = target_num_bits_internal; break;
        case 0b01: target_num_resources = target_num_qubits_internal; break;
        case 0b10: target_num_resources = target_num_bits_external; break;
        case 0b11: target_num_resources = target_num_qubits_external; break;
        default: ERROR("Scope and classification failed to pick target num of resources!");
    }

    #ifdef DEBUG
    std::cout << YELLOW("Creating resource definitions for " + owner + ", target num resources = " + std::to_string(target_num_resources)) << std::endl;
    std::cout << YELLOW("scope is external classification is qubit") << std::endl;
    std::cout << YELLOW(std::to_string((scope_is_external << 1) | classificaton_is_qubit)) << std::endl;
    #endif

    while(target_num_resources > 0){
        /*
            Use singular qubit or qubit register
        */
        if(random_int(1)){
            target_num_resources -= make_singular_resource_definition(scope, classification, total_num_definitions);

        } else {
            target_num_resources -= make_register_resource_definition(target_num_resources, scope, classification, total_num_definitions);
        }
    }

    return total_num_definitions;
}

/// @brief Given set of qubits from genome, create qubit definitions from those qubits. Note that the preset targets for the block are ignored
/// @param scope 
/// @param qubits 
/// @return 
unsigned int Block::make_resource_definitions(U8 scope, const Collection<Resource::Qubit>& _qubits){
    unsigned int num_definitions = 0;

    // only create definitions from qubits matching the scope
    qubits = _qubits;

    for(const Resource::Qubit& qubit : qubits){
        if(qubit.get_scope() & scope){
            num_definitions += qubit_to_qubit_def(scope, qubit);
        }
    }
    
    return num_definitions;
}

/// @brief Given set of bits from genome, create bit definitions from those bits. Note that the preset targets for the block are ignored
/// @param scope 
/// @param bits 
/// @return 
unsigned int Block::make_resource_definitions(U8 scope, const Collection<Resource::Bit>& _bits){
    unsigned int num_definitions = 0;

    // only create definitions from qubits matching the scope
    bits = _bits;

    for(const Resource::Bit& bit : bits){
        if(bit.get_scope() & scope){
            num_definitions += bit_to_bit_def(scope, bit);
        }
    }
    
    return num_definitions;
}


/// @brief Add qubit to set of qubit defs in block, merge with register as required
/// @param qubit 
unsigned int Block::qubit_to_qubit_def(const U8& scope, const Resource::Qubit& qubit){
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


/// @brief Add bit to set of bit defs in block, merge with register as required
/// @param bit 
unsigned int Block::bit_to_bit_def(const U8& scope, const Resource::Bit& bit){
    for(Bit_definition& bit_def : bit_defs){
        if(bit_def.defines(bit)){
            bit_def.increase_size();
            return 0;
        }
    }

    if(bit.is_register_def()){
        Register_bit_definition bit_def(bit);
        bit_defs.add(Bit_definition(bit_def, scope));

    } else {
        Singular_bit_definition bit_def(bit);
        bit_defs.add(Bit_definition(bit_def, scope));
    }

    return 1;
}

void Block::print_info() const {

    std::cout << "=======================================" << std::endl;
    std::cout << "              BLOCK INFO               " << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << "Owner: " << owner << std::endl;

    std::cout << "Target num qubits " << std::endl;
    std::cout << " EXTERNAL: " << target_num_qubits_external << std::endl;
    std::cout << " INTERNAL: " << target_num_qubits_internal << std::endl;

    std::cout << "Target num bits " << std::endl;
    std::cout << " EXTERNAL: " << target_num_bits_external << std::endl;
    std::cout << " INTERNAL: " << target_num_bits_internal << std::endl;

    std::cout << std::endl;
    std::cout << "Qubit definitions " << std::endl;

    if(owner == Common::TOP_LEVEL_CIRCUIT_NAME){
        std::cout << YELLOW("Qubit defs may not match target if block is built to match DAG") << std::endl;
    }

    for(const Qubit_definition& qubit_def : qubit_defs){
        std::cout << "name: " << qubit_def.get_name()->get_string() << " " ;

        if(qubit_def.is_register_def()){
            std::cout << "size: " << qubit_def.get_size()->get_string();
        }

        std::cout << " Scope: " << (qubit_def.is_external() ? "external" : "internal"); 

        std::cout << std::endl;
    }
    std::cout << "=======================================" << std::endl;

}
