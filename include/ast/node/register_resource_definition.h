#ifndef REGISTER_RESOURCE_DEFINITION_H
#define REGISTER_RESOURCE_DEFINITION_H

#include <node.h>
#include <register_resource.h>
#include <variable.h>
#include <integer.h>
#include <collection.h>

class Register_resource_definition : public Node {

    public:

        /// @brief Dummy resource definition
        Register_resource_definition() : 
            Node("Register_resource_definition", hash_rule_name("register_resource_definition"))
        {}

        Register_resource_definition(Variable _name, Integer _size, bool _is_qubit, bool _owned) : 
            Node(_is_qubit ? "register_qubit_def" : "register_bit_def", 
                 hash_rule_name(_is_qubit ? "register_qubit_def" : "register_bit_def")),
            name(_name),
            size(_size),
            resource_type(_is_qubit ? Resource::QUBIT : Resource::BIT),
            owned(_owned)
        {}

        std::shared_ptr<Variable> get_name(){
            return std::make_shared<Variable>(name);
        }

        std::shared_ptr<Integer> get_size(){
            return std::make_shared<Integer>(size);
        }

        bool is_owned() const {
            return owned;
        }

        void set_owned(bool _owned) {
            owned = _owned;
        }

        /// @brief Add resources to the given vector from the register
        /// @param output 
        /// @param external
        /// @param _is_qubit
        void make_resources(Collection<Resource::Resource>& output, bool external, bool _is_qubit, bool _owned) const {
            size_t reg_size = safe_stoi(size.get_string()).value();

            for(size_t i = 0; i < reg_size; i++){
                Register_resource reg_resource(name, Integer(std::to_string(i)), _is_qubit, _owned);

                output.add(Resource::Resource(reg_resource, external, _owned));
            }
        }

    private:
        Variable name;
        Integer size;
        Resource::Resource_Classification resource_type;
        bool owned = false;

};


#endif

