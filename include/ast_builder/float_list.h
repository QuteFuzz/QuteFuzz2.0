#ifndef FLOAT_LIST_H
#define FLOAT_LIST_H

#include <node.h>

class Float_list : public Node {

    public:
        using Node::Node;

        Float_list(std::string str, U64 hash, int num_floats_in_list):
            Node(str, hash)
        {
            constraint = std::make_optional<Node_constraint>(Common::float_literal, num_floats_in_list);
        }

    private:

};

#endif