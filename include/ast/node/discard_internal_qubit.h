#ifndef DISCARD_INTERNAL_QUBIT_H
#define DISCARD_INTERNAL_QUBIT_H

#include <node.h>

class Discard_internal_qubit : public Node {

    public:
    
        Discard_internal_qubit(const std::shared_ptr<Qubit_definition>& current_qubit_definition): 
            Node("discard_internal_qubit", Common::discard_internal_qubit)
        {
            if (current_qubit_definition->is_register_def()) {
                add_constraint(Common::discard_single_qubit, 0);
            } else {
                add_constraint(Common::discard_qreg, 0);
            }
        }
    
    private:

};


#endif
