#ifndef COMPOUND_STMT_H
#define COMPOUND_STMT_H

#include <node.h>

class Compound_stmt : public Node {

    public:

        Compound_stmt(std::string str, U64 hash, int compound_stmt_depth):
            Node(str, hash, indentation_tracker)
        {
            if(compound_stmt_depth == 0){
                constraint = std::make_optional<Node_constraint>(Node_constraint({Common::qubit_op}, {1}));
            }
        }

    private:

};

#endif
