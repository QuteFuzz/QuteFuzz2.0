#ifndef FLOAT_H
#define FLOAT_H

#include <node.h>

class Float : public Node {

    public:
        using Node::Node;

        Float() :
            Node(std::to_string(random_float(10)))
        {}

        Float(float n) :
            Node(std::to_string(n)),
            num(n)
        {}

        float get_num() const {return num;}

    private:
        float num;

};


#endif
