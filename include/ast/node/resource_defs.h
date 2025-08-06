#ifndef RESOURCE_DEFS_H
#define RESOURCE_DEFS_H

#include <node.h>


class Resource_defs : public Node {

    public:

        Resource_defs(std::string str, U64 hash, size_t num_defs, bool external, bool is_qubit = true):
            Node(str, hash, indentation_tracker)
        {
            if(external){
                if (is_qubit) {
                    constraint = std::make_optional<Node_constraint>(Common::qubit_def_external, num_defs);
                } else {
                    constraint = std::make_optional<Node_constraint>(Common::bit_def_external, num_defs);
                }
            } else {
                if (is_qubit) {
                    constraint = std::make_optional<Node_constraint>(Common::qubit_def_internal, num_defs);
                } else {
                    constraint = std::make_optional<Node_constraint>(Common::bit_def_internal, num_defs);
                }
            }
        }

    private:

};


#endif
