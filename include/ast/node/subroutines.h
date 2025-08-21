#ifndef SUBROUTINES_H
#define SUBROUTINES_H

#include <node.h>

class Subroutines : public Node {

    public:
        using Node::Node;

        Subroutines(unsigned int n_blocks):
            Node("subroutines", Common::subroutines)
        {
            constraint = std::make_optional<Node_constraint>(Common::block, n_blocks);            
        }

    private:

};

#endif
