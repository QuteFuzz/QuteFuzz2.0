#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <node.h>

/*
    Used to override node print to add a space before each expression
*/

class Expression : public Node {

    public:

        Expression(std::string str, U64 hash):
            Node(str, hash)
        {}

        void print(std::ostream& stream) const override {
            stream << " ";
            
            for(const std::shared_ptr<Node>& child : children){
                stream << *child;
            } 
        }

    private:

};

#endif
