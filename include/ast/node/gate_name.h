#ifndef GATE_NAME_H
#define GATE_NAME_H

#include <node.h>

class Gate_name : public Node {

    public:
        using Node::Node;

        Gate_name(const std::shared_ptr<Node> parent, const std::shared_ptr<Block> current_block, const std::vector<Common::Rule_hash>& swarm_testing_gateset) :
            Node("gate_name", Common::gate_name)
        {

            if(current_block->num_owned_qubits() == 0){
                add_constraint(Common::measure_and_reset, 0);
                if (current_block->total_num_bits() == 0) {
                    add_constraint(Common::Measure, 0);
                }
                /*
                    measure_and_reset only needs to owned qubits, and guppy doesn't have bit resources
                */
            }

            if(*parent == Common::subroutine_op){
                add_constraint(Common::subroutine, 1);
                
            } else if (*parent == Common::gate_op) {
                if(Common::swarm_testing){
                    set_constraint(swarm_testing_gateset, std::vector<unsigned int>(swarm_testing_gateset.size(), 0));
                    /* 
                        Might contain constraints with hash 0ULL if number of gates is less than SWARM_TESTING_GATESET_SIZE.
                        But since currently is determining non-occurance of gates, this is fine.
                    */
                }
                
                add_constraint(Common::subroutine, 0);                

            } else {
                ERROR("Gate name expected parent to be subroutine_op or gate_op!");
            }

        }

    private:

};

#endif