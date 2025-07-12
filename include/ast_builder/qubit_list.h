#ifndef QUBIT_LIST_H
#define QUBIT_LIST_H

#include <node.h>

class Qubit_list : public Node {

    public:

        Qubit_list(std::string str, U64 hash, int num_qubits_in_list, int indent_depth):
            Node(str, hash, indent_depth)
        {
            constraint = std::make_optional<Size_constraint>(Common::qubit, num_qubits_in_list);
        }

    private:

};

#endif
