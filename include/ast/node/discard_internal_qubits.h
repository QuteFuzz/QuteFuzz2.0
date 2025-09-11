#ifndef DISCARD_INTERNAL_QUBITS_H
#define DISCARD_INTERNAL_QUBITS_H

#include <node.h>

class Discard_internal_qubits : public Node {

    public:
    
        Discard_internal_qubits(unsigned int num_owned_qubit_defs): 
            Node("discard_internal_qubits")
        {
            // add_constraint(Common::discard_internal_qubit, num_owned_qubit_defs);
        }
    
    private:

};


#endif
