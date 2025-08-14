#ifndef REGISTER_RESOURCE_DEFINITION_H
#define REGISTER_RESOURCE_DEFINITION_H

#include <node.h>
#include <register_resource.h>
#include <variable.h>
#include <integer.h>
#include <collection.h>
#include <resource.h>

class Register_resource_definition : public Node {

    public:

        /// @brief Dummy resource definition
        Register_resource_definition() : 
            Node("")
        {}

        Register_resource_definition(Variable _name, Integer _size): 
            Node("register_resource_def", hash_rule_name("register_resource_def")),
            name(_name),
            size(_size)
        {}

        std::shared_ptr<Variable> get_name(){
            return std::make_shared<Variable>(name);
        }

        std::shared_ptr<Integer> get_size(){
            return std::make_shared<Integer>(size);
        }

    protected:
        Variable name;
        Integer size;
};

class Register_qubit_definition : public Register_resource_definition {

    public:
        Register_qubit_definition(Variable _name, Integer _size):
            Register_resource_definition(
                _name,
                _size
            )
        {}

        void make_resources(Collection<Resource::Qubit>& output, U8 scope) const {
            size_t reg_size = safe_stoi(size.get_string()).value();

            for(size_t i = 0; i < reg_size; i++){
                Register_qubit reg_qubit(name, Integer(std::to_string(i)));

                output.add(Resource::Qubit(reg_qubit, scope));
            }
        }

    private:

};

class Register_bit_definition : public Register_resource_definition {

    public:
        Register_bit_definition(Variable _name, Integer _size):
            Register_resource_definition(
                _name,
                _size
            )
        {}

        void make_resources(Collection<Resource::Bit>& output, U8 scope) const {
            size_t reg_size = safe_stoi(size.get_string()).value();

            for(size_t i = 0; i < reg_size; i++){
                Register_bit reg_bit(name, Integer(std::to_string(i)));

                output.add(Resource::Bit(reg_bit, scope));
            }
        }

    private:

};

#endif

