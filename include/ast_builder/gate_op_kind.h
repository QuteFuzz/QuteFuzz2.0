#ifndef GATE_OP_KIND_H
#define GATE_OP_KIND_H

#include <node.h>

class Gate_op_kind : public Node {

    public:

        Gate_op_kind(std::string str, U64 hash, int num_gate_params):
            Node(str, hash)
        {
            if(num_gate_params >= 1){
                constraint = std::make_optional<Node_constraint>(Common::float_list, 1);

            } else {
                constraint = std::make_optional<Node_constraint>(Common::float_list, 0);
            }
        }

    private:

};

#endif
