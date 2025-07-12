#ifndef REGISTER_QUBIT_DEFINITION_INT_H
#define REGISTER_QUBIT_DEFINITION_INT_H

#include <node.h>
#include <register_qubit.h>
#include <variable.h>
#include <integer.h>

class Register_qubit_definition_int : public Node {

    public:

        Register_qubit_definition_int(Variable _name, Integer _size): 
            Node("internal_qubit_def_register", hash_rule_name("internal_qubit_def_register")),
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
        void make_qubits(std::vector<Qubit::Qubit>& block_qubits) const {
            size_t reg_size = safe_stoi(size.get_string()).value();

            for(size_t i = 0; i < reg_size; i++){
                Register_qubit reg_qubit(name, Integer(std::to_string(i)));

                block_qubits.push_back(Qubit::Qubit(reg_qubit));
            }
        }

    private:
        Variable name;
        Integer size;

};


#endif

