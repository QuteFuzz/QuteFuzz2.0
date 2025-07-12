#ifndef EXTERNAL_QUBIT_DEFS_H
#define EXTERNAL_QUBIT_DEFS_H

#include <node.h>

class External_qubit_defs : public Node {

    public:

        External_qubit_defs(std::string str, U64 hash, int num_defs, int indent_depth):
            Node(str, hash, indent_depth)
        {
            constraint = std::make_optional<Size_constraint>(Common::qubit_def, num_defs);
        }

    private:

};

#endif
