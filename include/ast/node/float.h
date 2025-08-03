#ifndef FLOAT_H
#define FLOAT_H

#include <node.h>

class Float : public Node {

    public:
        using Node::Node;

        Float() :
            Node(std::to_string(random_float(10)))         
        {}

    private:

};


#endif
