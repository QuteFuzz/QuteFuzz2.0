#ifndef GATE_NAME_H
#define GATE_NAME_H

#include <node.h>

class Gate_name : public Node {

    public:
        using Node::Node;

        Gate_name(const std::shared_ptr<Node> parent, const std::shared_ptr<Block> current_block, const std::array<std::optional<Common::Rule_hash>, Common::SWARM_TESTING_GATESET_SIZE>& swarm_testing_gateset) :
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
                add_constraint(Common::subroutine, 0);

                if(Common::swarm_testing){
                    if(std::all_of(swarm_testing_gateset.begin(), swarm_testing_gateset.end(), [](const std::optional<Common::Rule_hash>& g){ return g.has_value(); })) {
                        add_constraint(swarm_testing_gateset[random_int(Common::SWARM_TESTING_GATESET_SIZE-1)].value(), 1);
                    } else {
                        // If swarm_testing_gateset is not full, allow all gates except for the ones already in the gateset
                        for(int i = 0; i < Common::SWARM_TESTING_GATESET_SIZE; i++){
                            if(swarm_testing_gateset[i].has_value()){
                                add_constraint(swarm_testing_gateset[i].value(), 0);
                            }
                        }
                    }
                }

                /* 
                    Swarm testing begins with populating swarm_testing_gateset with unique gates at runtime, until it is full. Then it will choose from this set only.
                */

            } else {
                ERROR("Gate name expected parent to be subroutine_op or gate_op!");
            }

        }

    private:

};

#endif