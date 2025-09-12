#ifndef GATE_OP_KIND_H
#define GATE_OP_KIND_H

#include <node.h>

class Gate_op_kind : public Node {

    public:

        Gate_op_kind(std::string str, U64 hash, int num_gate_params, int num_bits, int num_arguments):
            Node(str, hash)
        {

            bool has_gate_params = num_gate_params > 0;
            bool has_bits = num_bits > 0;
            bool has_arguments = num_arguments > 0;

            add_constraint(Common::float_list, has_gate_params ? 1 : 0);
            add_constraint(Common::bit_list, has_bits ? 1 : 0);
            add_constraint(Common::arguments, has_arguments ? 1 : 0);
        }

    private:

};

#endif
