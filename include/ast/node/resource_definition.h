#ifndef RESOURCE_DEFINITION_H
#define RESOURCE_DEFINITION_H

#include <register_resource_definition.h>
#include <singular_resource_definition.h>

class Resource_definition : public Node {

    public:

        /// @brief Dummy definition
        Resource_definition() : 
            Node("resource_def", hash_rule_name("resource_def")),
            value(Register_resource_definition()), 
            scope(Resource::Scope::EXTERNAL)
        {}

        Resource_definition(Register_resource_definition def, Resource::Scope _scope) :
            Node("resource_def", hash_rule_name("resource_def")),
            value(def), 
            scope(_scope)
        {}

        Resource_definition(Singular_resource_definition def, Resource::Scope _scope) :
            Node("resource_def", hash_rule_name("resource_def")),
            value(def), 
            scope(_scope)
        {}

        U8 get_scope() { return Resource::to_u8(scope); }

        inline std::shared_ptr<Variable> get_name(){
            return std::visit([](auto&& val) -> std::shared_ptr<Variable> {
                return val.get_name();
            }, value);
        }

        inline std::shared_ptr<Integer> get_size(){                
            if(is_register_def()){
                return std::get<Register_resource_definition>(value).get_size();   
            }

            ERROR("Singular resource definitions do not have sizes!");

            return std::make_shared<Integer>();
        }

        inline bool is_external() const {
            return Resource::is_external(scope);
        }

        inline bool is_owned() const {
            return Resource::is_owned(scope);
        }

        inline bool is_register_def() const {
            return std::holds_alternative<Register_resource_definition>(value);
        }

    private:
        std::variant<Register_resource_definition, Singular_resource_definition> value;
        Resource::Scope scope;

};

class Qubit_definition : public Resource_definition {

    public:
        Qubit_definition() : Resource_definition() {}

        Qubit_definition(Register_resource_definition def, Resource::Scope scope):
            Resource_definition(
                def, 
                scope
            )
        {
            if (Resource::is_external(scope)) {
                constraint = std::make_optional<Node_constraint>(Common::register_qubit_def_external, 1);
            } else {
                constraint = std::make_optional<Node_constraint>(Common::register_qubit_def_internal, 1);
            }
        }

        Qubit_definition(Singular_resource_definition def, Resource::Scope scope):
            Resource_definition(
                def, 
                scope
            )
        {
            if (scope == Resource::Scope::EXTERNAL || scope == Resource::Scope::EXTERNAL_OWNED) {
                constraint = std::make_optional<Node_constraint>(Common::singular_qubit_def_external, 1);
            } else {
                constraint = std::make_optional<Node_constraint>(Common::singular_qubit_def_internal, 1);
            }
        }

    private:

};

class Bit_definition : public Resource_definition {

    public:
        Bit_definition() : Resource_definition() {}

        Bit_definition(Register_resource_definition def, Resource::Scope scope):
            Resource_definition(
                def, 
                scope
            )
        {
            if (scope == Resource::Scope::EXTERNAL) {
                constraint = std::make_optional<Node_constraint>(Common::register_bit_def_external, 1);
            } else {
                constraint = std::make_optional<Node_constraint>(Common::register_bit_def_internal, 1);
            }
        }

        Bit_definition(Singular_resource_definition def, Resource::Scope scope):
            Resource_definition(
                def, 
                scope
            )
        {
            if (scope == Resource::Scope::EXTERNAL) {
                constraint = std::make_optional<Node_constraint>(Common::singular_bit_def_external, 1);
            } else {
                constraint = std::make_optional<Node_constraint>(Common::singular_bit_def_internal, 1);
            }
        }

    private:

};

#endif
