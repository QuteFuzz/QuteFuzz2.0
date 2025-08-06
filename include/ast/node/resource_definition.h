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
            type(Resource::REGISTER_EXTERNAL),
            resource_type(Resource::QUBIT)
        {}

        Resource_definition(Register_resource_definition def, bool external, bool is_qubit, bool _owned) :
            Node(is_qubit ? "qubit_def" : "bit_def", 
                 hash_rule_name(is_qubit ? "qubit_def" : "bit_def")),
            value(def), 
            type(external ? Resource::REGISTER_EXTERNAL : Resource::REGISTER_INTERNAL),
            resource_type(is_qubit ? Resource::QUBIT : Resource::BIT),
            owned(_owned)
        {
            if (external) {
                if (is_qubit) {
                    constraint = std::make_optional<Node_constraint>(Common::register_qubit_def_external, 1);
                } else {
                    constraint = std::make_optional<Node_constraint>(Common::register_bit_def_external, 1);
                }
            } else {
                if (is_qubit) {
                    constraint = std::make_optional<Node_constraint>(Common::register_qubit_def_internal, 1);
                } else {
                    constraint = std::make_optional<Node_constraint>(Common::register_bit_def_internal, 1);
                }
            }
        }

        Resource_definition(Singular_resource_definition def, bool external, bool is_qubit, bool _owned) :
            Node(is_qubit ? "qubit_def" : "bit_def", 
                 hash_rule_name(is_qubit ? "qubit_def" : "bit_def")),
            value(def), 
            type(external ? Resource::SINGULAR_EXTERNAL : Resource::SINGULAR_INTERNAL),
            resource_type(is_qubit ? Resource::QUBIT : Resource::BIT),
            owned(_owned)
        {
            if (external) {
                if (is_qubit) {
                    constraint = std::make_optional<Node_constraint>(Common::singular_qubit_def_external, 1);
                } else {
                    constraint = std::make_optional<Node_constraint>(Common::singular_bit_def_external, 1);
                }
            } else {
                if (is_qubit) {
                    constraint = std::make_optional<Node_constraint>(Common::singular_qubit_def_internal, 1);
                } else {
                    constraint = std::make_optional<Node_constraint>(Common::singular_bit_def_internal, 1);
                }
            }
        }

        Resource::Resource_Type get_type() { return type; }

        bool is_qubit() const { return resource_type == Resource::QUBIT; }

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
            return ((type == Resource::REGISTER_EXTERNAL) || (type == Resource::SINGULAR_EXTERNAL));
        }

        inline bool is_owned() const {
            return owned;
        }

        inline void set_owned(bool _owned) {
            owned = _owned;
        }

        inline bool is_register_def() const {
            return ((type == Resource::REGISTER_EXTERNAL) || (type == Resource::REGISTER_INTERNAL));
        }

    private:
        std::variant<Register_resource_definition, Singular_resource_definition> value;
        Resource::Resource_Type type;
        Resource::Resource_Classification resource_type;
        bool owned = false;

};

#endif
