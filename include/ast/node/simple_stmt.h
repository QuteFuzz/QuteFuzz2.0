#ifndef SIMPLE_STMT_H
#define SIMPLE_STMT_H

#include <node.h>

class Simple_stmt : public Node {

    public:

        Simple_stmt():
            Node("simple_stmt", Common::simple_stmt, indentation_tracker)
        {}

    private:

};

#endif
