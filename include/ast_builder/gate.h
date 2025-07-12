#ifndef GATE_H
#define GATE_H

#include <node.h>
#include <graph.h>

class Gate : public Node {
    public:
        
        Gate(const std::string str, int _qubits, int _params, std::shared_ptr<Graph> qig = nullptr) :
            Node(str),
            num_qubits(_qubits),
            num_params(_params),
            best_entanglement(
                (qig != nullptr) ?
                qig->get_best_entanglement(num_qubits) :
                std::nullopt
            )
        {}

        int get_num_qubits(){
            return num_qubits;
        }

        int get_num_params(){
            return num_params;
        }

        std::optional<std::vector<int>> get_best_entanglement(){
            return best_entanglement;
        }

    private:
        int num_qubits;
        int num_params;
        std::optional<std::vector<int>> best_entanglement;

};


#endif
