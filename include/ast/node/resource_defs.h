#ifndef RESOURCE_DEFS_H
#define RESOURCE_DEFS_H

#include <node.h>


class Qubit_defs : public Node {

    public:

        Qubit_defs(std::string str, U64 hash, size_t num_defs, U8 scope):
            Node(str, hash, indentation_tracker)
        {
            if(Resource::is_external(scope)){
                constraint = std::make_optional<Node_constraint>(Node_constraint({Common::qubit_def_external}, {num_defs}));
            } else {
                constraint = std::make_optional<Node_constraint>(Node_constraint({Common::qubit_def_internal}, {num_defs}));
            }

        }

    private:

};

class Bit_defs : public Node {

    public:

        Bit_defs(std::string str, U64 hash, size_t num_defs, U8 scope):
            Node(str, hash, indentation_tracker)
        {
            if(Resource::is_external(scope)){
                constraint = std::make_optional<Node_constraint>(Node_constraint({Common::bit_def_external}, {num_defs}));
            } else {
                constraint = std::make_optional<Node_constraint>(Node_constraint({Common::bit_def_internal}, {num_defs}));
            }

        }

    private:

};

#endif
