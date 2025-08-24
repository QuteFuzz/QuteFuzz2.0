#ifndef GATE_NAME_H
#define GATE_NAME_H

#include <node.h>

class Gate_name : public Node {

    public:
        using Node::Node;

        Gate_name(const std::shared_ptr<Node> parent, const std::shared_ptr<Block> current_block):
            Node("gate_name", Common::gate_name)
        {
            bool can_measure = (current_block->num_owned_qubits() == 0) || (current_block->total_num_bits() == 0);

            if(!can_measure){
                add_constraint(Common::Measure, 0);
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