#ifndef REGISTER_QUBIT_DEFINITION_H
#define REGISTER_QUBIT_DEFINITION_H

#include <node.h>
#include <register_qubit.h>
#include <variable.h>
#include <integer.h>
#include <collection.h>

class Register_qubit_definition : public Node {

    public:

        /// dummy qubit def
        Register_qubit_definition() : 
            Node("register_qubit_def", hash_rule_name("register_qubit_def"))
        {}

        Register_qubit_definition(Variable _name, Integer _size): 
            Node("register_qubit_def", hash_rule_name("register_qubit_def")),
            name(_name),
            size(_size)
        {}

        std::shared_ptr<Variable> get_name(){
            return std::make_shared<Variable>(name);
        }

        std::shared_ptr<Integer> get_size(){
            return std::make_shared<Integer>(size);
        }

        /// @brief Add qubits to the given vector from the qreg
        /// @param qubits 
        void make_qubits(Collection<Qubit::Qubit>& output, bool external) const {
            size_t reg_size = safe_stoi(size.get_string()).value();

            for(size_t i = 0; i < reg_size; i++){
                Register_qubit reg_qubit(name, Integer(std::to_string(i)));

                output.add(Qubit::Qubit(reg_qubit, external));
            }
        }

    private:
        Variable name;
        Integer size;

};


#endif

