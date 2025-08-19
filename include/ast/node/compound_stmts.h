#ifndef COMPOUND_STMTS_H
#define COMPOUND_STMTS_H

#include <node.h>

class Compound_stmts : public Node {

    public:

        Compound_stmts(int num_statements):
            Node("compound_stmts", Common::compound_stmts)
        {
            constraint = std::make_optional<Node_constraint>(Common::compound_stmt, num_statements);
        }

    private:

};

#endif
