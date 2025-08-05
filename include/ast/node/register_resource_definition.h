#ifndef REGISTER_RESOURCE_DEFINITION_H
#define REGISTER_RESOURCE_DEFINITION_H

#include <node.h>
#include <register_resource.h>
#include <variable.h>
#include <integer.h>
#include <collection.h>

namespace Register_resource_definition {

    
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
            void make_qubits(Collection<Resource::Resource>& output, bool external) const {
                size_t reg_size = safe_stoi(size.get_string()).value();

                for(size_t i = 0; i < reg_size; i++){
                    Register_resource::Register_qubit reg_qubit(name, Integer(std::to_string(i)));

                    output.add(Resource::Resource(reg_qubit, external));
                }
            }

        private:
            Variable name;
            Integer size;

    };

    class Register_bit_definition : public Node {

        public:

            /// dummy bit def
            Register_bit_definition() : 
                Node("register_bit_def", hash_rule_name("register_bit_def"))
            {}

            Register_bit_definition(Variable _name, Integer _size): 
                Node("register_bit_def", hash_rule_name("register_bit_def")),
                name(_name),
                size(_size)
            {}

            std::shared_ptr<Variable> get_name(){
                return std::make_shared<Variable>(name);
            }

            std::shared_ptr<Integer> get_size(){
                return std::make_shared<Integer>(size);
            }

            void make_bits(Collection<Resource::Resource>& output, bool external) const {
                size_t reg_size = safe_stoi(size.get_string()).value();

                for(size_t i = 0; i < reg_size; i++){
                    Register_resource::Register_bit reg_bit(name, Integer(std::to_string(i)));

                    output.add(Resource::Resource(reg_bit, external));
                }
            }

        private:
            Variable name;
            Integer size;

    };

}


#endif

