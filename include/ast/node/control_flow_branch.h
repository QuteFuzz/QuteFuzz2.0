#ifndef CONTROL_FLOW_BRANCH_H
#define CONTROL_FLOW_BRANCH_H

/*
    elif and else make up this node type
    these need to keep the indentation tracker in order to indent correctly
*/

#include <node.h>

class Control_flow_branch : public Node {

    public:

        Control_flow_branch(std::string str, U64 hash):
            Node(str, hash, indentation_tracker)
        {}

        void print(std::ostream& stream) const override {
            stream << indentation_str;

            for(const std::shared_ptr<Node>& child : children){
                stream << *child;
            } 
        }

    private:

};

#endif
