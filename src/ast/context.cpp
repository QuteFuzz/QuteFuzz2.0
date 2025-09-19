#include <context.h>
#include <generator.h>

namespace Context {

    void Context::reset(Level l){

        if(l == PROGRAM){
            subroutine_counter = 0;
            Node::node_counter = 0;
            can_copy_dag = false;

            blocks.clear();

            subroutines_node = std::nullopt;
            genome = std::nullopt;

        } else if (l == BLOCK){
            nested_depth = Common::NESTED_MAX_DEPTH;

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

    unsigned int Context::get_max_external_qubits(){
        size_t res = Common::MIN_QUBITS;

        for(const std::shared_ptr<Block>& block : blocks){
            res = std::max(res, block->num_external_qubits());
        }

        return res;
    }

    unsigned int Context::get_max_external_bits(){
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
        INFO("Current block owner: " + current_block_owner);
        INFO("Number of blocks: " + std::to_string(blocks.size()));
        #endif
        
        while(block->owned_by(Common::TOP_LEVEL_CIRCUIT_NAME) || 
            block->owned_by(current_block_owner) ||
            (block->num_external_qubits() > current_block->total_num_qubits()) ||
            (block->num_external_bits() > current_block->total_num_bits()) //TODO: Add check for other parameters if needed
        )
        {
            // INFO("Trying out block owned by " + block->get_owner() + " with " + std::to_string(block->num_external_qubits()) + " external qubits and " + std::to_string(block->num_external_bits()) + " external bits");
            block = blocks.at(random_int(blocks.size()-1));
        }

        return block;
    }

    std::shared_ptr<Block> Context::new_block_node(std::string str, U64 hash){
        std::shared_ptr<Block> current_block;

        reset(BLOCK);

        if(current_block_is_subroutine()){

            if(genome.has_value()){
                if (Common::cross_qss) {
                    current_block_owner = genome->subroutine_dags.at(subroutine_counter).get_name();
                    current_block = std::make_shared<Block>(str, hash, current_block_owner, genome->subroutine_dags.at(subroutine_counter++).get_qubits().get_total());

                    can_copy_dag = true;
                } else {
                    std::shared_ptr<Node> subroutine = genome.value().dag.get_next_subroutine_gate();

                    std::cout << YELLOW("setting block from DAG ") << std::endl;
                    std::cout << YELLOW("owner: " + subroutine->get_string()) << std::endl; 
                    std::cout << YELLOW("n ports: " + std::to_string(subroutine->get_n_ports())) << std::endl; 

                    current_block_owner = subroutine->get_string();
                    current_block = std::make_shared<Block>(str, hash, current_block_owner, subroutine->get_n_ports());
                }

            } else {
                current_block_owner = "sub"+std::to_string(subroutine_counter++);
                current_block = std::make_shared<Block>(str, hash, current_block_owner);
            }

        } else {
            current_block_owner = Common::TOP_LEVEL_CIRCUIT_NAME;
            current_block = std::make_shared<Block>(str, hash, Common::TOP_LEVEL_CIRCUIT_NAME);
            current_block->add_constraint(Common::non_comptime_block, 0);

            subroutine_counter = 0;

            can_copy_dag = genome.has_value();
        }

        blocks.push_back(current_block);

        return current_block;
    }

    std::shared_ptr<Qubit_defs> Context::get_qubit_defs_node(std::string str, U64 hash){
        std::shared_ptr<Block> current_block = get_current_block();

        U8 scope;

        switch(hash){
            case Common::qubit_defs_external: 
                scope = EXTERNAL_SCOPE;
                break;
            case Common::qubit_defs_internal:
                scope = INTERNAL_SCOPE | OWNED_SCOPE;
                break;
            case Common::qubit_defs_external_owned:
                scope = EXTERNAL_SCOPE | OWNED_SCOPE;
                break;
            default:
                scope = EXTERNAL_SCOPE;
                ERROR("Unknown qubit defs hash: " + std::to_string(hash));
        }

        size_t num_defs;

        if(can_copy_dag){
            // Iterate through subroutine dags in genome if current block is a subroutine
            if (Common::cross_qss && genome.has_value() && !current_block->owned_by(Common::TOP_LEVEL_CIRCUIT_NAME)) {
                //Find the right subroutine dag using name and iterating through the array
                for (const auto& sub_dag : genome->subroutine_dags) {
                    if (sub_dag.get_name() == current_block_owner) {
                        current_subroutine_dag = sub_dag;
                        break;
                    }
                }
                
                num_defs = current_block->make_resource_definitions(scope, current_subroutine_dag->get_qubits());

            } else {
                num_defs = current_block->make_resource_definitions(scope, genome->dag.get_qubits());
                /*
                    guppy main circuit needs only define internal qubits, so at the very least it can use the subcircuits,
                    even if the previous subroutines don't.

                    Also, external owned qubits of pytket triggers the qubit_to_qubit_def twice for each qubit in guppy, since
                    pytket external owned qubits are both internal and external at the same time to guppy. This needs a little change
                */

            }
        
        } else {
            num_defs = current_block->make_resource_definitions(scope, Resource::QUBIT);
        }
        
        
        return std::make_shared<Qubit_defs>(str, hash, num_defs, scope);
    }

    std::shared_ptr<Bit_defs> Context::get_bit_defs_node(std::string str, U64 hash){
        std::shared_ptr<Block> current_block = get_current_block();

        U8 scope = (hash == Common::bit_defs_external) ? EXTERNAL_SCOPE : INTERNAL_SCOPE;

        size_t num_defs;
        
        if(can_copy_dag){
            num_defs = current_block->make_resource_definitions(scope, genome->dag.get_bits());
            // TODO: Once Bit defs are supported for cross-qss testing, iterate through subroutine dags in genome
        } else {
            num_defs = current_block->make_resource_definitions(scope, Resource::BIT);
        }
    
        return std::make_shared<Bit_defs>(str, hash, num_defs, scope);
    }

    std::optional<std::shared_ptr<Block>> Context::get_block(std::string owner){
        for(const std::shared_ptr<Block>& block : blocks){
            if(block->owned_by(owner)) return std::make_optional<std::shared_ptr<Block>>(block); 
        }

        INFO("Block owner by " + owner + " not defined!");

        return std::nullopt;
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

    std::shared_ptr<Nested_branch> Context::get_nested_branch(const std::string& str, const U64& hash, std::shared_ptr<Node> parent){
        if(can_copy_dag){
            return std::make_shared<Nested_branch>(str, hash, parent->get_next_qubit_op_target());

        } else {
            return std::make_shared<Nested_branch>(str, hash);
        }
    }

    std::shared_ptr<Nested_stmt> Context::get_nested_stmt(const std::string& str, const U64& hash, std::shared_ptr<Node> parent){
        nested_depth -= 1;

        if(can_copy_dag){
            return std::make_shared<Nested_stmt>(str, hash, parent->get_next_qubit_op_target());

        } else {
            return std::make_shared<Nested_stmt>(str, hash);
        }
    }

    std::shared_ptr<Compound_stmt> Context::get_compound_stmt(std::shared_ptr<Node> parent){
        
        if(can_copy_dag){
            return Compound_stmt::from_num_qubit_ops(parent->get_next_qubit_op_target());
        } else {
            return Compound_stmt::from_nested_depth(nested_depth);
        }
    
    }

    std::shared_ptr<Compound_stmts> Context::get_compound_stmts(std::shared_ptr<Node> parent){
        
        /*
            this check is to make sure we only call the function for the compound statements rule in the circuit body, as opposed to each nested
            call within the body in control flow
        */

        if(*parent == Common::body){
            set_can_apply_subroutines();

            if(can_copy_dag){
                if (Common::cross_qss && !get_current_block()->owned_by(Common::TOP_LEVEL_CIRCUIT_NAME)) {
                    parent->make_partition(current_subroutine_dag->n_qubit_ops(), 1);
                } else {
                    parent->make_partition(genome.value().dag.n_qubit_ops(), 1);
                }
                
            }
        }

        if(can_copy_dag){
            return Compound_stmts::from_num_qubit_ops(parent->get_next_qubit_op_target());

        } else {
            return Compound_stmts::from_num_compound_stmts(WILDCARD_MAX);
        }   
    }

    std::shared_ptr<Subroutines> Context::new_subroutines_node(){
        unsigned int n_blocks = random_int(Common::MAX_SUBROUTINES);

        if(genome.has_value()){
            if (Common::cross_qss) {
                n_blocks = genome.value().subroutine_dags.size();
            } else {
                n_blocks = genome.value().dag.n_subroutines();
            }
            /* 
            These should be the same things, so may be possible to replace
            genome.value().dag.n_subroutines() with genome.value().subroutine_dags.size()
            */
        }

        std::shared_ptr<Subroutines> node = std::make_shared<Subroutines>(n_blocks);

        subroutines_node = std::make_optional<std::shared_ptr<Subroutines>>(node);

        return node;
    }


    int Context::get_current_gate_num_params(){
        if(current_gate != nullptr){
            return current_gate->get_num_params();
        } else {
            WARNING("Current gate not set but trying to get num params! 1 parameter will be returned");
            return 1;
        }
    }

    int Context::get_current_gate_num_qubit_params(){
        if(current_gate != nullptr){
            return current_gate->get_num_qubit_params();
        } else {
            WARNING("Current gate not set but trying to get num qubit params! 0 qubit parameters will be returned");
            return 0;
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

    void Context::set_genome(const std::optional<Genome>& _genome){
        genome = _genome;
    }

}