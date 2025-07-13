#ifndef SIMPLE_STMT_H
#define SIMPLE_STMT_H

#include <node.h>

class Simple_stmt : public Node {

    public:

        Simple_stmt(std::string str, U64 hash):
            Node(str, hash, indentation_tracker)
        {}

    private:

};

#endif
