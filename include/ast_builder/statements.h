#ifndef STATEMENTS_H
#define STATEMENTS_H

#include <node.h>

class Statements : public Node {

    public:

        Statements(std::string str, U64 hash, int num_statements):
            Node(str, hash)
        {
            constraint = std::make_optional<Size_constraint>(Common::statement, num_statements);
        }

    private:

};

#endif
