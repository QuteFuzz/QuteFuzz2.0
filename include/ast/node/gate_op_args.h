#ifndef GATE_OP_ARGS_H
#define GATE_OP_ARGS_H

#include <node.h>
#include <gate.h>

class Gate_op_args : public Node {

    public:

        Gate_op_args(const std::shared_ptr<Gate> current_gate):
            Node("gate_op_args", Token::GATE_OP_ARGS)
        {

            if(*current_gate == Token::SUBROUTINE){
                ERROR("Gate op args cannot be used on subroutine!");
            
            } else {
                add_constraint(Token::FLOAT_LIST, current_gate->get_num_floats() > 0);
                add_constraint(Token::BIT_LIST, current_gate->get_num_external_bits() > 0);
            }

        }

    private:

};

#endif
