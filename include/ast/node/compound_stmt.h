#ifndef COMPOUND_STMT_H
#define COMPOUND_STMT_H

#include <node.h>

class Gate;
class Block;

class Compound_stmt : public Node {

    public:
        Compound_stmt(unsigned int control_flow_depth);

    private:
};

#endif
