#ifndef CONJUNCTION_H
#define CONJUNCTION_H

#include <node.h>


/*
    Used to override node print to add spaces after each child
*/

class Conjunction : public Node {

    public:
        Conjunction() :
            Node("conjuction",  Common::conjunction)         
        {
            add_constraint(Common::inversion, 2);
        }

        void print(std::ostream& stream) const override {
            for(const std::shared_ptr<Node>& child : children){
                stream << *child << " ";
            } 
        }

    private:

};


#endif
