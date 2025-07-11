#ifndef INTERNAL_QUBIT_DEFS_H
#define INTERNAL_QUBIT_DEFS_H

#include <node.h>

class Internal_qubit_defs : public Node {

    public:

        Internal_qubit_defs(std::string str, U64 hash, int num_defs):
            Node(str, hash)
        {
            constraint = std::make_optional<Size_constraint>(Common::internal_qubit_def, num_defs);
        }

    private:

};

#endif
