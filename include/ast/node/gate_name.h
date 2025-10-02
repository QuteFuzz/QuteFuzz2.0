#ifndef GATE_NAME_H
#define GATE_NAME_H

#include <node.h>

class Gate_name : public Node {

    public:
        using Node::Node;

        Gate_name(const std::shared_ptr<Node> parent, const std::shared_ptr<Block> current_block, const std::optional<Node_constraint>& swarm_testing_gateset) :
            Node("gate_name", Token::GATE_MAME, swarm_testing_gateset)
        {

            if(current_block->num_qubits_of(OWNED_SCOPE) == 0){
                add_constraint(Token::MEASURE_AND_RESET, 0);

                if (current_block->num_bits_of(ALL_SCOPES) == 0) {
                    add_constraint(Token::MEASURE, 0);
                }
                /*
                    measure_and_reset only needs owned qubits, and guppy doesn't have bit resources
                */
            }

            if(*parent == Token::SUBROUTINE_OP){
                add_constraint(Token::SUBROUTINE, 1);
                
            } else if (*parent == Token::GATE_OP) {                
                add_constraint(Token::SUBROUTINE, 0);                

            } else {
                ERROR("Gate name expected parent to be subroutine_op or gate_op!");
            }

        }

    private:

};

#endif