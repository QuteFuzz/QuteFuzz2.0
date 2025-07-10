#ifndef INTEGER_H
#define INTEGER_H

#include <node.h>

class Integer : public Node {

    public:
        using Node::Node;

        Integer() : 
            Node("42")
        {}

    private:

};


#endif
