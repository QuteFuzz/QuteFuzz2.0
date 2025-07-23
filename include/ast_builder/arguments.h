#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <node.h>

class Arguments : public Node {

    public:

        Arguments(std::string str, U64 hash, int num_arguments):
            Node(str, hash)
        {
            constraint = std::make_optional<Size_constraint>(Common::arg, num_arguments);
        }

    private:

};

#endif
