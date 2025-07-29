#include <context.h>

namespace Context {

    void Context::reset(Level l){

        if(l == PROGRAM){
            subroutines_node = nullptr;
            subroutine_counter = 0;
            blocks.clear();

        } else if (l == BLOCK){
            compound_stmt_depth = Common::COMPOUND_STMT_DEPTH;

        } else if (l == QUBIT_OP){
            get_current_block()->qubit_flag_reset();
        }
    }

    void Context::set_can_apply_subroutines(bool override_flag){
        std::shared_ptr<Block> current_block = get_current_block();

        if (current_block->get_can_apply_subroutines() && override_flag) {
            for(std::shared_ptr<Block> block : blocks){
                if (!block->owned_by(Common::TOP_LEVEL_CIRCUIT_NAME) &&
                    !block->owned_by(current_block_owner) &&
                    (block->num_external_qubits() <= current_block->total_num_qubits())
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

    std::shared_ptr<Block> Context::get_current_block() const {
        if(blocks.size()) {
            return blocks.back();
        } else {
            return std::make_shared<Block>(dummy_block);
        }
    }

    /// @brief This is completely safe. If this function is called, then `set_can_apply_subroutines` must've passed
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
            (block->num_external_qubits() > current_block->total_num_qubits())
        )
        {
            block = blocks.at(random_int(blocks.size()-1));
        }

        return block;
    }

    std::shared_ptr<Block> Context::setup_block(std::string str, U64 hash){

        int target_num_qubits_external;
        int target_num_qubits_internal;

        if(current_block_is_subroutine()){
            current_block_owner = "sub"+std::to_string(subroutine_counter++);
            target_num_qubits_external = random_int(Common::MAX_QUBITS, Common::MIN_QUBITS);
            target_num_qubits_internal = random_int(Common::MAX_QUBITS-target_num_qubits_external); 

        } else {
            current_block_owner = Common::TOP_LEVEL_CIRCUIT_NAME;
            //This is assuming that the main circuit either has all its qubits defined internally or externally, not both
            int max_qubits = get_max_defined_qubits();
            target_num_qubits_external = max_qubits;
            target_num_qubits_internal = max_qubits;

            subroutine_counter = 0;
        }

        std::shared_ptr<Block> current_block = std::make_shared<Block>(str, hash, current_block_owner, target_num_qubits_external, target_num_qubits_internal);
        blocks.push_back(current_block);

        return current_block;
    }

    std::shared_ptr<Qubit_defs> Context::make_qubit_definitions(std::string& str, U64& hash){
        std::shared_ptr<Block> current_block = get_current_block();

        bool external = (hash == Common::qubit_defs_external);

        size_t num_defs = current_block->make_qubit_definitions(external);

        return std::make_shared<Qubit_defs>(str, hash, num_defs, external);  
    }

    std::shared_ptr<Block> Context::get_block(std::string owner){
        for(const std::shared_ptr<Block>& block : blocks){
            if(block->owned_by(owner)) return block; 
        }

        INFO("Block owner by " + owner + " not defined!");

        return nullptr;
    }

    std::shared_ptr<Arg::Arg> Context::get_current_arg(const std::string& str, const U64& hash){
        if(current_gate != nullptr){
            std::shared_ptr<Qubit_definition::Qubit_definition> qubit_def = current_applied_block->get_next_external_qubit_def();
            Arg::Type arg_qubit_def_type = qubit_def->get_type() == Qubit_definition::Type::SINGULAR_EXTERNAL ? Arg::Type::SINGULAR : Arg::Type::REGISTER;
            current_applied_block_qubit_def_size = (arg_qubit_def_type == Arg::Type::SINGULAR) ? 1 : std::stoi(qubit_def->get_size()->get_string());
            return std::make_shared<Arg::Arg>(str, hash, arg_qubit_def_type);
        } else {
            WARNING("Current gate not set but trying to get arg! Using dummy instead");
            return std::make_shared<Arg::Arg>();
        }
    }

    std::shared_ptr<Qubit::Qubit> Context::get_current_qubit(){
        current_qubit = get_current_block()->get_random_qubit();     
        return current_qubit;
    }

    std::shared_ptr<Integer> Context::get_current_qubit_index(){
        if(current_qubit != nullptr){
            return current_qubit->get_index();
        } else {
            WARNING("Current qubit not set but trying to get index! Using dummy instead");
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

    std::shared_ptr<Qubit_definition::Qubit_definition> Context::get_current_qubit_definition(){
        current_qubit_definition = get_current_block()->get_next_qubit_def();
        return current_qubit_definition;
    }

    std::shared_ptr<Discard_qubit_def> Context::get_current_qubit_definition_discard(const std::string& str, const U64& hash){
        current_qubit_definition = get_current_block()->get_next_owned_qubit_def();
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

    std::shared_ptr<Variable> Context::get_current_qubit_definition_name(){
        if(current_qubit_definition != nullptr){
            return current_qubit_definition->get_name();
        } else {
            WARNING("Current qubit not set but trying to get name! Using dummy instead");
            return std::make_shared<Variable>(dummy_var);
        }
    }

    std::shared_ptr<Gate> Context::make_current_gate(const std::string& str, int num_qubits, int num_params) {
        current_gate = std::make_shared<Gate>(str, num_qubits, num_params);
        return current_gate;
    }

    std::shared_ptr<Discard_qubit_defs> Context::discard_qubit_defs(const std::string& str, const U64& hash, int num_owned_qubit_defs) {
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
            WARNING("Current gate not set but trying to get num params! Assumed 1 parameter");
            return 1;
        }
    }

    int Context::get_current_gate_num_qubits(){
        if(current_gate != nullptr){
            return current_gate->get_num_qubits();
        } else {
            WARNING("Current gate not set but trying to get num params! Assumed 1 qubit");
            return 1;
        }
    }

    void Context::set_current_applied_block(){
        current_applied_block = get_block(current_gate->get_string());
        if(current_applied_block == nullptr){
            ERROR("Current applied block not set! Assumed current gate is function");
        }
    }

}