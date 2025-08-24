#ifndef FLOAT_LIST_H
#define FLOAT_LIST_H

#include <node.h>

class Float_list : public Node {

    public:
        using Node::Node;

        Float_list(std::string str, U64 hash, unsigned int num_floats_in_list):
            Node(str, hash)
        {
            add_constraint(Common::float_literal, num_floats_in_list);
        }

    private:

};

#endif