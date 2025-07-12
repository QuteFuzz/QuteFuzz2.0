#ifndef QUBIT_DEFS_H
#define QUBIT_DEFS_H

#include <node.h>

class Qubit_defs : public Node {

    public:

        Qubit_defs(std::string str, U64 hash, int num_defs, int indent_depth, bool external = true):
            Node(str, hash, indent_depth)
        {
            if(external){
                constraint = std::make_optional<Size_constraint>(Common::qubit_def_external, num_defs);
            } else {
                constraint = std::make_optional<Size_constraint>(Common::qubit_def_internal, num_defs);

            }
        }

    private:

};

#endif
