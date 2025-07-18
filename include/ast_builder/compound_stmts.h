#ifndef COMPOUND_STMTS_H
#define COMPOUND_STMTS_H

#include <node.h>

class Compound_stmts : public Node {

    public:

        Compound_stmts(std::string str, U64 hash, int num_statements):
            Node(str, hash)
        {
            constraint = std::make_optional<Size_constraint>(Common::compound_stmt, num_statements);
        }

    private:

};

#endif
