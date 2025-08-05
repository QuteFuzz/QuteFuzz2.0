#ifndef RESOURCE_LIST_H
#define RESOURCE_LIST_H

#include <node.h>

namespace Resource_list {

    class Qubit_list : public Node {

    public:

        Qubit_list(std::string str, U64 hash, int num_qubits_in_list):
            Node(str, hash)
        {
            constraint = std::make_optional<Node_constraint>(Common::qubit, num_qubits_in_list);
        }

    private:

    };
    

    class Bit_list : public Node {
    public:

        Bit_list(std::string str, U64 hash, int num_bits_in_list):
            Node(str, hash)
        {
            constraint = std::make_optional<Node_constraint>(Common::bit, num_bits_in_list);
        }
        
    private:
    
    };

}

#endif
