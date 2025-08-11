#ifndef INTEGER_H
#define INTEGER_H

#include <node.h>

class Integer : public Node {

    public:
        using Node::Node;

        Integer() : 
            Node(std::to_string(random_int(42)))
        {}

        Integer(int i) : 
            Node(std::to_string(i))
        {}

    private:

};


#endif
