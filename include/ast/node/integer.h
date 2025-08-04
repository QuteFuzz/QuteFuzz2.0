#ifndef INTEGER_H
#define INTEGER_H

#include <node.h>

class Integer : public Node {

    public:
        using Node::Node;

        Integer() : 
            Node(std::to_string(random_int(42)))
        {}

    private:

};


#endif
