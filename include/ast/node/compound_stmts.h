#ifndef COMPOUND_STMTS_H
#define COMPOUND_STMTS_H

#include <node.h>

class Compound_stmts : public Node {

    public:

        Compound_stmts(unsigned int num_statements):
            Node("compound_stmts", Common::compound_stmts)
        {
            add_constraint(Common::compound_stmt, num_statements);
        }

    private:

};

#endif
