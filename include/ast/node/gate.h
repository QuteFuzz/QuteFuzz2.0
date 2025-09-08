#ifndef GATE_H
#define GATE_H

#include <node.h>

class Gate : public Node {

    public:

        Gate() : 
            Node("dummy")
        {}
        
        Gate(const std::string& str, U64 _hash, unsigned int _qubits, unsigned int _bits, unsigned int _params, unsigned int _qubit_params = 0) :
            Node(str),
            num_qubits(_qubits),
            num_bits(_bits),
            num_params(_params),
            num_qubit_params(_qubit_params)
        {
            hash = _hash;
        }

        std::string get_id_as_str(){
            return std::to_string(id);
        }

        unsigned int get_num_qubits(){
            return num_qubits;
        }

        unsigned int get_num_bits(){
            return num_bits;
        }

        unsigned int get_num_params(){
            return num_params;
        }

        unsigned int get_num_qubit_params(){
            return num_qubit_params;
        }

        unsigned int get_n_ports() const override {return num_qubits ? num_qubits : num_qubit_params;}
        /*
            This is a weird quirk with guppy where the external qubits are passed in as parameters, which means the
            number of ports a subroutine gate has is the number of qubits it takes in as parameters + the number of qubits it operates on
        */

    private:
        unsigned int num_qubits;
        unsigned int num_bits;
        unsigned int num_params;
        unsigned int num_qubit_params;
};


#endif
