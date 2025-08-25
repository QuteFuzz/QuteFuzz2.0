#ifndef SUBROUTINES_H
#define SUBROUTINES_H

#include <node.h>

class Subroutines : public Node {

    public:

        Subroutines(unsigned int n_blocks):
            Node("subroutines", Common::subroutines)
        {
            add_constraint(Common::block, n_blocks);
        }

    private:

};

#endif
