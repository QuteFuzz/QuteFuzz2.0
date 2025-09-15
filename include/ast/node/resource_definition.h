#ifndef RESOURCE_DEFINITION_H
#define RESOURCE_DEFINITION_H

#include <register_resource_definition.h>
#include <singular_resource_definition.h>

class Resource_definition : public Node {

    public:

        /// @brief Dummy definition
        Resource_definition() : 
            Node("resource_def", hash_rule_name("resource_def")),
            value(Register_resource_definition()) 
        {}

        Resource_definition(const Register_resource_definition& def, const U8& _scope) :
            Node("resource_def", hash_rule_name("resource_def")),
            value(def), 
            scope(_scope)
        {}

        Resource_definition(const Singular_resource_definition& def, const U8& _scope) :
            Node("resource_def", hash_rule_name("resource_def")),
            value(def), 
            scope(_scope)
        {}

        U8 get_scope() const { return scope; }

        inline std::shared_ptr<Variable> get_name() const {
            return std::visit([](auto&& val) -> std::shared_ptr<Variable> {
                return val.get_name();
            }, value);
        }

        /// @brief Get size of resource definition if it is a register definition. If not, return 1, or whatever `default_size` is as an `Integer` node
        /// @param default_size 
        /// @return 
        inline std::shared_ptr<Integer> get_size(unsigned int default_size = 1) const {                
            if(is_register_def()){
                return std::get<Register_resource_definition>(value).get_size();   
            }

            WARNING("Singular resource definitions do not have sizes! Using default size = " + default_size);

            return std::make_shared<Integer>(default_size);
        }

        inline bool is_register_def() const {
            return std::holds_alternative<Register_resource_definition>(value);
        }

        inline bool defines(const Resource::Resource& resource) const {
            return get_name()->get_string() == resource.get_name()->get_string();
        }

        inline void increase_size(){
            if(is_register_def()) std::get<Register_resource_definition>(value).increase_size();
        }

    private:
        std::variant<Register_resource_definition, Singular_resource_definition> value;
        U8 scope;

};

class Qubit_definition : public Resource_definition {

    public:
        Qubit_definition() : Resource_definition() {}

        Qubit_definition(const Register_resource_definition& def, const U8& scope):
            Resource_definition(
                def, 
                scope
            )
        {
            add_constraint(Common::register_qubit_def, 1);
        }

        Qubit_definition(const Singular_resource_definition& def, const U8& scope):
            Resource_definition(
                def, 
                scope
            )
        {
            add_constraint(Common::singular_qubit_def, 1);
        }

    private:

};

class Bit_definition : public Resource_definition {

    public:
        Bit_definition() : Resource_definition() {}

        Bit_definition(const Register_resource_definition& def, const U8& scope):
            Resource_definition(
                def, 
                scope
            )
        {
            add_constraint(Common::register_bit_def, 1);
        }

        Bit_definition(const Singular_resource_definition& def, const U8& scope):
            Resource_definition(
                def, 
                scope
            )
        {
            add_constraint(Common::singular_bit_def, 1);
        }

    private:

};

#endif
