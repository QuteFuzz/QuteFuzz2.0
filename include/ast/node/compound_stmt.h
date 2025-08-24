#ifndef COMPOUND_STMT_H
#define COMPOUND_STMT_H

#include <node.h>

class Gate;
class Block;

class Compound_stmt : public Node {

    public:
        Compound_stmt(int compound_stmt_depth);
        
        std::string resolved_name() const override;

    private:
};

#endif
