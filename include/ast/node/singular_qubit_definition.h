#ifndef SINGULAR_QUBIT_DEFINITION_H
#define SINGULAR_QUBIT_DEFINITION_H

#include <node.h>

class Singular_qubit_definition : public Node {

    public:

        /// dummy qubit def
        Singular_qubit_definition() : 
            Node("singular_qubit_def", hash_rule_name("singular_qubit_def"))
        {}

        Singular_qubit_definition(Variable _name) : 
            Node("singular_qubit_def", hash_rule_name("singular_qubit_def")),
            name(_name)
        {}

        std::shared_ptr<Variable> get_name(){
            return std::make_shared<Variable>(name);
        }

        void make_qubits(Collection<Qubit::Qubit>& output, bool external) const {
            Singular_qubit singular_qubit(name);
            output.add(Qubit::Qubit(singular_qubit, external));
        }

    private:
        Variable name;

};


#endif

