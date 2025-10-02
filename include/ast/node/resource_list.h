#ifndef RESOURCE_LIST_H
#define RESOURCE_LIST_H

#include <node.h>

class Qubit_list : public Node {

    public:
        Qubit_list(unsigned int num_qubits_in_list):
            Node("qubit_list", Token::QUBIT_LIST)
        {
            add_constraint(Token::QUBIT, num_qubits_in_list);
        }

    private:

};

class Bit_list : public Node {

    public:
        Bit_list(unsigned int num_bits_in_list):
            Node("bit_list", Token::BIT_LIST)
        {
            add_constraint(Token::BIT, num_bits_in_list);
        }

    private:

};

#endif
