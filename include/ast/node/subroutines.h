#ifndef SUBROUTINES_H
#define SUBROUTINES_H

#include <node.h>

class Subroutines : public Node {

    public:
        using Node::Node;

        Subroutines(std::string str, U64 hash):
            Node(str, hash)
        {
            constraint = std::make_optional<Node_constraint>(Node_constraint({Common::block}, {size_t(random_int(Common::MAX_SUBROUTINES))}));
        }

    private:

};

#endif
