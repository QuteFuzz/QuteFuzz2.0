#ifndef BIT_LIST_H
#define BIT_LIST_H

#include <node.h>

class Bit_list : public Node {

    public:

        Bit_list(std::string str, U64 hash, int num_bits_in_list):
            Node(str, hash)
        {
            constraint = std::make_optional<Node_constraint>(Common::bit, num_bits_in_list);
        }

    private:

};

#endif
