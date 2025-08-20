#ifndef GATE_OP_KIND_H
#define GATE_OP_KIND_H

#include <node.h>

class Gate_op_kind : public Node {

    public:

        Gate_op_kind(std::string str, U64 hash, int num_gate_params, int num_bits):
            Node(str, hash)
        {

            bool has_gate_params = num_gate_params > 0;
            bool has_bits = num_bits > 0;

            if(has_gate_params && !has_bits){
                constraint = std::make_optional<Node_constraint>(Common::float_list, 1);

            } else if (!has_gate_params && has_bits){
                constraint = std::make_optional<Node_constraint>(Common::bit_list, 1);

            } else if (!has_gate_params && !has_bits) {
                constraint = std::make_optional<Node_constraint>(Node_constraint({Common::float_list, Common::bit_list}, {0, 0}));

            }
        }

    private:

};

#endif
