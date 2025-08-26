#ifndef INTEGER_H
#define INTEGER_H

#include <node.h>

class Integer : public Node {

    public:
        using Node::Node;

        Integer() : 
            Node(std::to_string(42))
        {}

        Integer(int i) : 
            Node(std::to_string(i)),
            num(i)
        {}

        void operator++(int){
            num += 1;
            string = std::to_string(num);
        }

    private:
        int num;

};


#endif
