#ifndef RESOURCE_DEFS_H
#define RESOURCE_DEFS_H

#include <node.h>

class Qubit_defs : public Node {

    public:

        Qubit_defs(unsigned int num_defs):
            Node(indentation_tracker)
        {
            add_constraint(Common::qubit_def, num_defs);
        }

    private:

};

class Bit_defs : public Node {

    public:

        Bit_defs(unsigned int num_defs):
            Node(indentation_tracker)
        {
            add_constraint(Common::bit_def, num_defs);
        }

    private:

};

#endif
