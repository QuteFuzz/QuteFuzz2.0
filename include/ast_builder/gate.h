#ifndef GATE_H
#define GATE_H

#include <node.h>

class Gate : public Node {
    static int index_tracker;

    public:
        
        Gate(const std::string str, int _qubits, int _params) :
            Node(str),
            num_qubits(_qubits),
            num_params(_params),
            index(index_tracker++)
        {}

        std::string get_index_as_str(){
            return std::to_string(index);
        }

        int get_num_qubits(){
            return num_qubits;
        }

        int get_num_params(){
            return num_params;
        }

    private:
        int num_qubits;
        int num_params;
        int index;
};


#endif
