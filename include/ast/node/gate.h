#ifndef GATE_H
#define GATE_H

#include <node.h>

class Gate : public Node {

    public:

        Gate() : 
            Node("dummy")
        {}
        
        Gate(const std::string& str, U64 _hash, unsigned int _qubits, unsigned int _bits, unsigned int _params) :
            Node(str),
            num_qubits(_qubits),
            num_bits(_bits),
            num_params(_params)
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

        unsigned int get_n_ports() const override {return num_qubits;}

    private:
        unsigned int num_qubits;
        unsigned int num_bits;
        unsigned int num_params;
};


#endif
