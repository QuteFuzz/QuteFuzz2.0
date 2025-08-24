#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <node.h>

/*
    Used to override node print to add a space before each expression
*/

class Expression : public Node {

    public:

        Expression():
            Node("expression", Common::expression)
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
