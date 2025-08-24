#ifndef GATE_NAME_H
#define GATE_NAME_H

#include <node.h>

class Gate_name : public Node {

    public:
        using Node::Node;

        Gate_name(const std::shared_ptr<Node> parent, const std::shared_ptr<Block> current_block):
            Node("gate_name", Common::gate_name)
        {
            bool cannot_measure = (current_block->num_owned_qubits() == 0) || (current_block->total_num_bits() == 0);

            if(cannot_measure){
                add_constraint(Common::Measure, 0);
                /*
                    because the number of occurances = 0, even though these rules are technically not expected to appear in the same grammar, 
                    we can add constraints for both, becuase we're checking for non-occurance
                */
                add_constraint(Common::measure_and_reset, 0);
            }

            if(*parent == Common::subroutine_op){
                add_constraint(Common::subroutine, 1);
            } else if (*parent == Common::gate_op) {
                add_constraint(Common::subroutine, 0);
            } else {
                ERROR("Gate name expected parent to be subroutine_op or gate_op!");
            }

        }

    private:

};

#endif