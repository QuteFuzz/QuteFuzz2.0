#include <context.h>
#include <gate.h>

namespace Context {

    void Context::reset(Level l){

        if(l == PROGRAM){
            qig = nullptr;
            subroutines_node = nullptr;
            subroutine_counter = 0;
            blocks.clear();
        
        } else if (l == QUBIT_OP){
            get_current_block()->qubit_flag_reset();
        }
    }

    void Context::set_can_apply_subroutines(){
        std::shared_ptr<Block> current_block = get_current_block();

        for(std::shared_ptr<Block> block : blocks){
            if(!block->owned_by(Common::TOP_LEVEL_CIRCUIT_NAME) &&
               !block->owned_by(current_block_owner) &&
                (block->num_external_qubits() <= current_block->num_external_qubits())
            )
            {
                INFO("Block " + current_block_owner + " can apply subroutines");
                current_block->set_can_apply_subroutines(true);
            }
        }
    }

    size_t Context::get_max_defined_qubits(){
        size_t res = Common::MIN_QUBITS;

        for(const std::shared_ptr<Block>& block : blocks){
            res = std::max(res, block->num_external_qubits());
        }

        return res;
    }

    std::shared_ptr<Block> Context::get_current_block() const {
        assert(blocks.size());
        return blocks.back();
    }

    /// @brief This is completely safe. If this function is called, then the check to see whether subroutines can be chosen must've passed
    /// So you just need to return a current block that's not the main block, or the current block being generated
    /// @return 
    std::shared_ptr<Block> Context::get_random_block(){
        std::shared_ptr<Block> block = blocks.at(random_int(blocks.size()-1));

        #ifdef DEBUG
        INFO("Getting random block");
        #endif

        while(block->owned_by(Common::TOP_LEVEL_CIRCUIT_NAME) || block->owned_by(current_block_owner)){                
            block = blocks.at(random_int(blocks.size()-1));
        }

        return block;
    }

    std::shared_ptr<Block> Context::setup_block(std::string str, U64 hash){

        int target_num_qubits;

        if(current_block_is_subroutine()){
            current_block_owner = "sub"+std::to_string(subroutine_counter++);
            target_num_qubits = random_int(Common::MAX_QUBITS, Common::MIN_QUBITS);

        } else {
            current_block_owner = Common::TOP_LEVEL_CIRCUIT_NAME;
            target_num_qubits = get_max_defined_qubits();

            subroutine_counter = 0;
        }

        std::shared_ptr<Block> current_block = std::make_shared<Block>(str, hash, current_block_owner, target_num_qubits);
        blocks.push_back(current_block);

        return current_block;
    }

    size_t Context::make_qubit_definitions(bool external){
        std::shared_ptr<Block> current_block = get_current_block();

        current_block->make_qubit_definitions(external);

        return current_block->num_qubit_definitions();
    }

    std::shared_ptr<Qubit::Qubit> Context::get_current_qubit(){
        assert(current_gate != nullptr);
        current_qubit = get_current_block()->get_random_qubit(current_gate->get_best_entanglement());
        return current_qubit;
    }

    std::shared_ptr<Integer> Context::get_current_qubit_index(){
        if(current_qubit != nullptr){
            return current_qubit->get_index();
        }

        throw std::runtime_error("Current qubit not set but trying to get index!");
    }

    std::shared_ptr<Variable> Context::get_current_qubit_name(){
        if(current_qubit != nullptr){
            return current_qubit->get_name();
        }

        throw std::runtime_error("Current qubit not set but trying to get name!");
    }

    std::shared_ptr<Qubit_definition::Qubit_definition> Context::get_current_qubit_definition(){
        current_qubit_definition = get_current_block()->get_next_qubit_def();
        return current_qubit_definition;
    }

    std::shared_ptr<Integer> Context::get_current_qubit_definition_size(){
        if(current_qubit_definition != nullptr){
            return current_qubit_definition->get_size();
        }

        throw std::runtime_error("Current qubit definition not set but trying to get size!");
    }

    std::shared_ptr<Variable> Context::get_current_qubit_definition_name(){
        if(current_qubit_definition != nullptr){
            return current_qubit_definition->get_name();
        }

        throw std::runtime_error("Current qubit definition not set but trying to get name!");
    }

    std::shared_ptr<Gate> Context::get_current_gate(std::string str, int num_qubits, int num_params){
        current_gate = std::make_shared<Gate>(str, num_qubits, num_params, qig);
        return current_gate;
    }

    int Context::get_current_gate_num_params(){
        if(current_gate != nullptr){
            return current_gate->get_num_params();
        }

        throw std::runtime_error("Current gate not set but trying to get num params!");
    }

    int Context::get_current_gate_num_qubits(){
        if(current_gate != nullptr){
            return current_gate->get_num_qubits();
        }

        throw std::runtime_error("Current gate not set but trying to get num qubits!");
    }

    void Context::set_qig(){

        if(qig != nullptr){
            // render old qig
            // TODO: get name from qig, then use that to search blocks for the correct block to pass to render_graph 
            // fs::path img_path = circuit_dir / (current_block_owner + ".png");
            // qig->render_graph(img_path, get_current_block());
        }
        
        qig = std::make_shared<Graph>(get_current_block()->num_external_qubits());
    }
}