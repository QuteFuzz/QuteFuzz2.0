#ifndef FLOAT_LIST_H
#define FLOAT_LIST_H

#include <node.h>

class Float_list : public Node {

    public:
        using Node::Node;

        Float_list(unsigned int num_floats_in_list):
            Node("float_list", Token::FLOAT_LIST)
        {
            add_constraint(Token::FLOAT_LITERAL, num_floats_in_list);
        }

    private:

};

#endif