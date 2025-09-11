#ifndef DISCARD_INTERNAL_QUBIT_H
#define DISCARD_INTERNAL_QUBIT_H

#include <node.h>

class Discard_internal_qubit : public Node {

    public:

        Discard_internal_qubit(const std::shared_ptr<Qubit_definition>& current_qubit_definition, const std::string& owner): 
            Node("discard_internal_qubit")
        {
            bool is_register = current_qubit_definition->is_register_def();
            bool is_maincirc = (owner == Common::TOP_LEVEL_CIRCUIT_NAME);

            /*
            Discarding internal qubits in the main function of guppy should be recorded using
            the result() function instead of just discarded or measured
            */

            // if (is_register && is_maincirc) {
            //     add_constraint(Common::measure_and_record_qreg, 1);
            // } else if (!is_register && is_maincirc) {
            //     add_constraint(Common::measure_and_record_single_qubit, 1);
            // } else if (is_register && !is_maincirc) {
            //     add_constraint(Common::discard_qreg, 1);
            // } else if (!is_register && !is_maincirc) {
            //     add_constraint(Common::discard_single_qubit, 1);
            // }
        }
    
    private:

};


#endif
