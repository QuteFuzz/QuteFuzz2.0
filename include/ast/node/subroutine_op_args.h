#ifndef SUBROUTINE_OP_ARGS_H
#define SUBROUTINE_OP_ARGS_H

#include <node.h>

class Subroutine_op_args : public Node {

    public:

        Subroutine_op_args(unsigned int num_arguments):
            Node("subroutine_op_args", Common::subroutine_op_args)
        {
            add_constraint(Common::subroutine_op_arg, num_arguments);
        }

    private:

};

#endif
