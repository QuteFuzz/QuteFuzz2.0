#ifndef DISCARD_QUBIT_DEFS_H
#define DISCARD_QUBIT_DEFS_H

#include <node.h>

class Discard_qubit_defs : public Node {

    public:

        Discard_qubit_defs(std::string str, U64 hash, int num_owned_qubit_defs):
            Node(str, hash, indentation_tracker)
        {
            constraint = std::make_optional<Node_constraint>(Common::discard_internal_qubit, num_owned_qubit_defs);
        }

    private:

};

#endif