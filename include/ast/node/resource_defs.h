#ifndef RESOURCE_DEFS_H
#define RESOURCE_DEFS_H

#include <node.h>

class Qubit_defs : public Node {

    public:

        Qubit_defs(unsigned int num_defs):
            Node("qubit_defs", Token::QUBIT_DEFS, indentation_tracker)
        {
            add_constraint(Token::QUBIT_DEF, num_defs);
        }

    private:

};

class Bit_defs : public Node {

    public:

        Bit_defs(unsigned int num_defs):
            Node("bit_defs", Token::BIT_DEFS, indentation_tracker)
        {
            add_constraint(Token::BIT_DEF, num_defs);
        }

    private:

};

#endif
