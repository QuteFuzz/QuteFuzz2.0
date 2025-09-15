#ifndef SUBROUTINES_H
#define SUBROUTINES_H

#include <node.h>

class Subroutines : public Node {

    public:

        Subroutines(unsigned int n_blocks):
            Node("subroutines", Common::subroutine_defs)
        {
            add_constraint(Common::block, n_blocks);
        }

    private:

};

#endif
