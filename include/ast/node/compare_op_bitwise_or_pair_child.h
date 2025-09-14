#ifndef COMPARE_OP_BITWISE_OR_PAIR_CHILD_H
#define COMPARE_OP_BITWISE_OR_PAIR_CHILD_H

#include <node.h>

/*
    Used to override node print to add spaces after each child. 
    NOTE: This node is used for all *children* of `compare_op_bitwise_or_pair` as they all require spaces after their children
    It it NOT used to denote the type of the `compare_op_bitwise_or_pair` node 
*/

class Compare_op_bitwise_or_pair_child : public Node {

    public:
        Compare_op_bitwise_or_pair_child(std::string str, U64 hash) :
            Node(str, hash)         
        {}

        void print(std::ostream& stream) const override {
            stream << " ";

            for(const std::shared_ptr<Node>& child : children){
                stream << *child << " ";
            } 
        }

    private:

};


#endif
