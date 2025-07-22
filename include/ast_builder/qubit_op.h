#ifndef QUBIT_OP_H
#define QUBIT_OP_H

#include <node.h>

class Qubit_op : public Node {

    public:

        Qubit_op(std::string str, U64 hash, bool can_use_subroutine):
            Node(str, hash)
        {
            if(can_use_subroutine == false){
                constraint = std::make_optional<Node_constraint>(Common::subroutine_op, 0);
            }
        }

    private:

};

#endif
