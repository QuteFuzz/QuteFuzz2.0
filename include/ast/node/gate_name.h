#ifndef GATE_NAME_H
#define GATE_NAME_H

#include <node.h>

class Gate_name : public Node {

    public:
        using Node::Node;

        Gate_name(const std::shared_ptr<Node> parent, const std::shared_ptr<Block> current_block, const std::optional<Node_constraint>& swarm_testing_gateset) :
            Node("gate_name", Common::gate_name, swarm_testing_gateset)
        {

            if(current_block->num_owned_qubits() == 0){
                add_constraint(Common::measure_and_reset, 0);
                add_constraint(Common::Measure, 0);
                
                /*
                    measure_and_reset only needs owned qubits, and guppy doesn't have bit resources
                */
            } else {
                if (current_block->total_num_bits() == 0) {
                    add_constraint(Common::Measure, 0);
                }
            }

            if(*parent == Common::subroutine_op){
                add_constraint(Common::subroutine, 1);
                
            } else if (*parent == Common::gate_op) {                
                add_constraint(Common::subroutine, 0);                

            } else {
                ERROR("Gate name expected parent to be subroutine_op or gate_op!");
            }

        }


        /*
            For making gate_node from DAG, without directly referencing the qubit_op from the DAG
        */
        Gate_name(Common::Rule_hash gate_name_hash) :
            Node("gate_name", Common::gate_name)
        {
            add_constraint(gate_name_hash, 1);
        }

    private:

};

#endif