#ifndef BIT_DEFS_H
#define BIT_DEFS_H

#include <node.h>

class Bit_defs : public Node {

    public:

        Bit_defs(std::string str, U64 hash, size_t num_defs, bool external):
            Node(str, hash, indentation_tracker)
        {
            if(external){
                constraint = std::make_optional<Node_constraint>(Common::bit_def_external, num_defs);
            } else {
                constraint = std::make_optional<Node_constraint>(Common::bit_def_internal, num_defs);

            }
        }

    private:

};

#endif
