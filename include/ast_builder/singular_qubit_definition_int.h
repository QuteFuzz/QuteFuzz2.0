#ifndef SINGULAR_QUBIT_DEFINITION_INT_H
#define SINGULAR_QUBIT_DEFINITION_INT_H

#include <node.h>

class Singular_qubit_definition_int : public Node {

    public:

        Singular_qubit_definition_int(Variable _name) : 
            Node("internal_qubit_def_singular", hash_rule_name("internal_qubit_def_singular")),
            name(_name)
        {}

        std::shared_ptr<Variable> get_name(){
            return std::make_shared<Variable>(name);
        }

        void make_qubits(std::vector<Qubit::Qubit>& block_qubits) const {
            Singular_qubit singular_qubit(name);
            block_qubits.push_back(Qubit::Qubit(singular_qubit));
        }

    private:
        Variable name;

};


#endif

