#ifndef QUBIT_OP_H
#define QUBIT_OP_H

#include <node.h>

class Qubit_op : public Node {

    public:

        Qubit_op(std::string str, U64 hash, bool can_use_subroutine, bool can_measure):
            Node(str, hash)
        {
            if(!can_use_subroutine && !can_measure){
                constraint = std::make_optional<Node_constraint>(Common::gate_op, 1);
            } else if (can_use_subroutine && !can_measure){
                constraint = std::make_optional<Node_constraint>(Common::measurement_op, 0);
            } else if (!can_use_subroutine && can_measure){
                constraint = std::make_optional<Node_constraint>(Common::subroutine_op, 0);
            }
        }

    private:

};

#endif
