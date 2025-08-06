#include <context.h>

namespace Context {

    void Context::reset(Level l){

        if(l == PROGRAM){
            subroutines_node = nullptr;
            subroutine_counter = 0;
            Node::node_counter = 0;
            blocks.clear();

        } else if (l == BLOCK){
            compound_stmt_depth = Common::COMPOUND_STMT_DEPTH;

        } else if (l == QUBIT_OP){
            get_current_block()->qubit_flag_reset();
            get_current_block()->bit_flag_reset();
            current_port = 0;
        }
    }

    void Context::set_can_apply_subroutines(bool override_flag){
        std::shared_ptr<Block> current_block = get_current_block();

        if (current_block->get_can_apply_subroutines() && override_flag) {
            for(std::shared_ptr<Block> block : blocks){
                if (!block->owned_by(Common::TOP_LEVEL_CIRCUIT_NAME) &&
                    !block->owned_by(current_block_owner) &&
                    (block->num_external_qubits() <= current_block->total_num_qubits()) &&
                    (block->num_external_bits() <= current_block->total_num_bits())
                )
                {
                    #ifdef DEBUG
                    INFO("Block " + current_block_owner + " can apply subroutines");
                    #endif
                    return;
                }
            }
        }

        #ifdef DEBUG
        INFO("Block " + current_block_owner + " can't apply subroutines");
        #endif
        current_block->set_can_apply_subroutines(false);
    }

    size_t Context::get_max_defined_qubits(){
        size_t res = Common::MIN_QUBITS;

        for(const std::shared_ptr<Block>& block : blocks){
            res = std::max(res, block->num_external_qubits());
        }

        return res;
    }

    size_t Context::get_max_defined_bits(){
        size_t res = Common::MIN_BITS;

        for(const std::shared_ptr<Block>& block : blocks){
            res = std::max(res, block->num_external_bits());
        }

        return res;
    }

    std::shared_ptr<Block> Context::get_current_block() const {
        if(blocks.size()) {
            return blocks.back();
        } else {
            return std::make_shared<Block>(dummy_block);
        }
    }

    /// @brief This loop is guaranteed to stop. If this function is called, then `set_can_apply_subroutines` must've passed
    /// So you just need to return a current block that's not the main block, or the current block and needs <= num qubits in current block
    /// Qubit comparison needed because `set_can_apply_subroutines` only tells you that there's at least one block that can be picked
    /// @return 
    std::shared_ptr<Block> Context::get_random_block(){
        std::shared_ptr<Block> block = blocks.at(random_int(blocks.size()-1));
        std::shared_ptr<Block> current_block = get_current_block();

        #ifdef DEBUG
        INFO("Getting random block");
        #endif
        
        while(block->owned_by(Common::TOP_LEVEL_CIRCUIT_NAME) || 
            block->owned_by(current_block_owner) ||
            (block->num_external_qubits() > current_block->total_num_qubits()) ||
            (block->num_external_bits() > current_block->total_num_bits()) //TODO: Add check for other parameters if needed
        )
        {
            block = blocks.at(random_int(blocks.size()-1));
        }

        return block;
    }

    std::shared_ptr<Block> Context::setup_block(std::string str, U64 hash){

        int target_num_qubits_external;
        int target_num_qubits_internal;
        int target_num_bits_external;
        int target_num_bits_internal;

        if(current_block_is_subroutine()){
            current_block_owner = "sub"+std::to_string(subroutine_counter++);
            target_num_qubits_external = random_int(Common::MAX_QUBITS, Common::MIN_QUBITS);
            target_num_qubits_internal = random_int(Common::MAX_QUBITS-target_num_qubits_external);
            target_num_bits_external = random_int(Common::MAX_BITS, Common::MIN_BITS);
            target_num_bits_internal = random_int(Common::MAX_BITS-target_num_bits_external);

        } else {
            current_block_owner = Common::TOP_LEVEL_CIRCUIT_NAME;
            //This is assuming that the main circuit either has all its qubits defined internally or externally, not both
            int max_qubits = get_max_defined_qubits();
            int max_bits = get_max_defined_bits();
            target_num_qubits_external = max_qubits;
            target_num_qubits_internal = max_qubits;
            target_num_bits_external = max_bits;
            target_num_bits_internal = max_bits;

            subroutine_counter = 0;
        }

        std::shared_ptr<Block> current_block = std::make_shared<Block>(str, hash, current_block_owner, target_num_qubits_external, target_num_qubits_internal, target_num_bits_external, target_num_bits_internal);
        blocks.push_back(current_block);

        return current_block;
    }

    std::shared_ptr<Resource_defs> Context::make_qubit_definitions(std::string& str, U64& hash){
        std::shared_ptr<Block> current_block = get_current_block();

        bool external = (hash == Common::qubit_defs_external);

        size_t num_defs = current_block->make_resource_definitions(external, true);

        return std::make_shared<Resource_defs>(str, hash, num_defs, external, true);
    }

    std::shared_ptr<Resource_defs> Context::make_bit_definitions(std::string& str, U64& hash){
        std::shared_ptr<Block> current_block = get_current_block();

        bool external = (hash == Common::bit_defs_external);

        size_t num_defs = current_block->make_resource_definitions(external, false);

        return std::make_shared<Resource_defs>(str, hash, num_defs, external, false);
    }

    std::optional<std::shared_ptr<Block>> Context::get_block(std::string owner){
        for(const std::shared_ptr<Block>& block : blocks){
            if(block->owned_by(owner)) return std::make_optional<std::shared_ptr<Block>>(block); 
        }

        INFO("Block owner by " + owner + " not defined!");

        return std::nullopt;
    }

    void Context::set_current_arg(const std::string& str, const U64& hash){
        if((current_gate != nullptr) && current_gate_definition.has_value()){
            std::shared_ptr<Resource_definition> qubit_def = current_gate_definition.value()->get_next_external_resource_def(true);
            current_arg = std::make_shared<Arg>(str, hash, qubit_def);
        }
    }

    std::shared_ptr<Arg> Context::get_current_arg(){
        return current_arg;
    }

    void Context::set_current_qubit(){
        Resource::Resource* random_qubit = get_current_block()->get_random_resource(current_gate->get_string()=="measure_and_reset", true); 
        
        random_qubit->extend_flow_path(current_gate, current_port++);

        current_qubit = std::make_shared<Resource::Resource>(*random_qubit);
    }

    void Context::set_current_bit(){
        Resource::Resource* random_bit = get_current_block()->get_random_resource(false, false);

        random_bit->extend_flow_path(current_gate, current_port++);

        current_bit = std::make_shared<Resource::Resource>(*random_bit);
    }

    std::shared_ptr<Resource::Resource> Context::get_current_qubit(){
        return current_qubit;
    }

    std::shared_ptr<Resource::Resource> Context::get_current_bit(){
        return current_bit;
    }

    std::shared_ptr<Integer> Context::get_current_qubit_index(){
        if(current_qubit != nullptr){
            return current_qubit->get_index();
        } else {
            WARNING("Current qubit not set but trying to get index! Using dummy instead");
            return std::make_shared<Integer>(dummy_int);
        }
    }

    std::shared_ptr<Integer> Context::get_current_bit_index(){
        if(current_bit != nullptr){
            return current_bit->get_index();
        } else {
            WARNING("Current bit not set but trying to get index! Using dummy instead");
            return std::make_shared<Integer>(dummy_int);
        }
    }

    std::shared_ptr<Variable> Context::get_current_qubit_name(){
        if(current_qubit != nullptr){
            return current_qubit->get_name();
        } else {
            WARNING("Current qubit not set but trying to get name! Using dummy instead");
            return std::make_shared<Variable>(dummy_var);
        }
    }

    std::shared_ptr<Variable> Context::get_current_bit_name(){
        if(current_bit != nullptr){
            return current_bit->get_name();
        } else {
            WARNING("Current bit not set but trying to get name! Using dummy instead");
            return std::make_shared<Variable>(dummy_var);
        }
    }

    void Context::set_current_qubit_definition(){
        current_qubit_definition = get_current_block()->get_next_resource_def(true);
    }

    void Context::set_current_bit_definition(){
        current_bit_definition = get_current_block()->get_next_resource_def(false);
    }

    std::shared_ptr<Resource_definition> Context::get_current_qubit_definition(){
        return current_qubit_definition;
    }

    std::shared_ptr<Resource_definition> Context::get_current_bit_definition(){
        return current_bit_definition;
    }

    void Context::set_current_qubit_definition_owned(){
        current_qubit_definition = get_current_block()->get_next_owned_resource_def(true);
    }

    void Context::set_current_bit_definition_owned(){
        current_bit_definition = get_current_block()->get_next_owned_resource_def(false);
    }

    std::shared_ptr<Discard_qubit_def> Context::get_current_qubit_definition_discard(const std::string& str, const U64& hash){
        return std::make_shared<Discard_qubit_def>(str, hash, current_qubit_definition);
    }

    std::shared_ptr<Integer> Context::get_current_qubit_definition_size(){
        if(current_qubit_definition != nullptr){
            return current_qubit_definition->get_size();
        } else  {
            WARNING("Current qubit not set but trying to get size! Using dummy instead");
            return std::make_shared<Integer>(dummy_int);
        }
    }

    std::shared_ptr<Integer> Context::get_current_qubit_definition_size_0_indexed(){
        if(current_qubit_definition != nullptr){
            return (current_qubit_definition->is_register_def() && current_qubit_definition->is_external()) ? current_qubit_definition->get_size() : std::make_shared<Integer>("0");
        } else {
            WARNING("Current qubit definition not set or is singular but trying to get size! Using dummy instead");
            return std::make_shared<Integer>(dummy_int);
        }
    }

    std::shared_ptr<Variable> Context::get_current_qubit_definition_name(){
        if(current_qubit_definition != nullptr){
            return current_qubit_definition->get_name();
        } else {
            WARNING("Current qubit not set but trying to get name! Using dummy instead");
            return std::make_shared<Variable>(dummy_var);
        }
    }

    std::shared_ptr<Integer> Context::get_current_bit_definition_size(){
        if(current_bit_definition != nullptr && current_bit_definition->is_register_def()){
            return current_bit_definition->get_size();
        } else {
            WARNING("Current bit definition not set or is singular but trying to get size! Using dummy instead");
            return std::make_shared<Integer>(dummy_int);
        }
    }

    std::shared_ptr<Variable> Context::get_current_bit_definition_name(){
        if(current_bit_definition != nullptr){
            return current_bit_definition->get_name();
        } else {
            WARNING("Current bit definition not set but trying to get name! Using dummy instead");
            return std::make_shared<Variable>(dummy_var);
        }
    }

    void Context::set_current_gate(const std::string& str, int num_qubits, int num_bits, int num_params){
        current_gate = std::make_shared<Gate>(str, num_qubits, num_bits, num_params);
    }

    std::shared_ptr<Gate> Context::get_current_gate(){
        return current_gate;
    }

    std::shared_ptr<Discard_qubit_defs> Context::get_discard_qubit_defs(const std::string& str, const U64& hash, int num_owned_qubit_defs) {
        return std::make_shared<Discard_qubit_defs>(str, hash, num_owned_qubit_defs);
    }

    std::shared_ptr<Node> Context::get_control_flow_stmt(const std::string& str, const U64& hash){
        compound_stmt_depth -= 1;
        return std::make_shared<Node>(str, hash);
    }

    std::shared_ptr<Compound_stmt> Context::get_compound_stmt(const std::string& str, const U64& hash){
        return std::make_shared<Compound_stmt>(str, hash, compound_stmt_depth);
    }

    int Context::get_current_gate_num_params(){
        if(current_gate != nullptr){
            return current_gate->get_num_params();
        } else {
            WARNING("Current gate not set but trying to get num params! 1 parameter will be returned");
            return 1;
        }
    }

    int Context::get_current_gate_num_qubits(){
        if(current_gate != nullptr){
            return current_gate->get_num_qubits();
        } else {
            WARNING("Current gate not set but trying to get num params! 1 qubit will be returned");
            return 1;
        }
    }

    int Context::get_current_gate_num_bits(){
        if(current_gate != nullptr){
            return current_gate->get_num_bits();
        } else {
            WARNING("Current gate not set but trying to get num bits! 1 bit will be returned");
            return 1;
        }
    }

    void Context::set_current_gate_definition(){
        current_gate_definition = get_block(current_gate->get_string());
    }

}