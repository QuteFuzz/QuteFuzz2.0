#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <node.h>

class Arguments : public Node {

    public:

        Arguments(unsigned int num_arguments):
            Node("arguments", Common::arguments)
        {
            add_constraint(Common::arg, num_arguments);
        }

    private:

};

#endif
