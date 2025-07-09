#ifndef QUBIT_LIST_H
#define QUBIT_LIST_H

#include <node.h>

class Qubit_list : public Node {

    public:

        Qubit_list(std::string str, U64 hash, int num_qubits_in_list):
            Node(str, hash)
        {
            constraint = std::make_optional<Size_constraint>(Common::qubit, num_qubits_in_list);
        }

    private:

};

#endif
