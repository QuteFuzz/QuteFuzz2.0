#ifndef RESOURCE_LIST_H
#define RESOURCE_LIST_H

#include <node.h>

class Qubit_list : public Node {

    public:
        Qubit_list(unsigned int num_qubits_in_list):
            Node("qubit_list", Common::qubit_list)
        {
            add_constraint(Common::qubit, num_qubits_in_list);
        }

    private:

};

class Bit_list : public Node {

    public:
        Bit_list(unsigned int num_bits_in_list):
            Node("bit_list", Common::bit_list)
        {
            add_constraint(Common::bit, num_bits_in_list);
        }

    private:

};

#endif
