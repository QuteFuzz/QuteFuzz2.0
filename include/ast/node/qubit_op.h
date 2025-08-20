#ifndef QUBIT_OP_H
#define QUBIT_OP_H

#include <node.h>

class Qubit_op : public Node {

    public:

        Qubit_op(std::string str, U64 hash, std::shared_ptr<Block> current_block):
            Node(str, hash)
        {
            bool can_use_subroutine = current_block->get_can_apply_subroutines();

            if(!can_use_subroutine){
                constraint = std::make_optional<Node_constraint>(Common::subroutine_op, 0);
            }
        }

    private:

};

#endif
