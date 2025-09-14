#ifndef FLOAT_LIST_H
#define FLOAT_LIST_H

#include <node.h>

class Float_list : public Node {

    public:
        using Node::Node;

        Float_list(unsigned int num_floats_in_list):
            Node("float_list", Common::float_list)
        {
            add_constraint(Common::float_literal, num_floats_in_list);
        }

    private:

};

#endif