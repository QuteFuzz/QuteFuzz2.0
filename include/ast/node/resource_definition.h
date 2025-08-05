#ifndef RESOURCE_DEFINITION_H
#define RESOURCE_DEFINITION_H

#include <register_resource_definition.h>
#include <singular_resource_definition.h>

namespace Resource_definition {

    enum Type {
        REGISTER_INTERNAL,
        SINGULAR_INTERNAL,
        REGISTER_EXTERNAL,
        SINGULAR_EXTERNAL
    };

    class Qubit_definition : public Node {

        public:

            /// @brief Dummy definition
            Qubit_definition() : 
                Node("qubit_def", hash_rule_name("qubit_def")),
                value(Register_resource_definition::Register_qubit_definition()), 
                type(REGISTER_EXTERNAL)
            {}

            Qubit_definition(Register_resource_definition::Register_qubit_definition def, bool external) :
                Node("qubit_def", hash_rule_name("qubit_def")),
                value(def), 
                type(external ? REGISTER_EXTERNAL : REGISTER_INTERNAL)
            {
                if (external) {
                    constraint = std::make_optional<Node_constraint>(Common::register_qubit_def_external, 1);
                } else {
                    constraint = std::make_optional<Node_constraint>(Common::register_qubit_def_internal, 1);
                }
            }

            Qubit_definition(Singular_resource_definition::Singular_qubit_definition def, bool external) :
                Node("qubit_def", hash_rule_name("qubit_def")),
                value(def), 
                type(external ? SINGULAR_EXTERNAL : SINGULAR_INTERNAL)
            {
                if (external) {
                    constraint = std::make_optional<Node_constraint>(Common::singular_qubit_def_external, 1);
                } else {
                    constraint = std::make_optional<Node_constraint>(Common::singular_qubit_def_internal, 1);
                }
            }

            Type get_type(){return type;}

            inline std::shared_ptr<Variable> get_name(){
                return std::visit([](auto&& val) -> std::shared_ptr<Variable> {
                    return val.get_name();
                }, value);
            }

            inline std::shared_ptr<Integer> get_size(){                
                if(is_register_def()){
                    return std::get<Register_resource_definition::Register_qubit_definition>(value).get_size();   
                }

                ERROR("Singular qubit definitions do not have sizes!");

                return std::make_shared<Integer>();
            }

            inline bool is_external() const {
                return ((type == REGISTER_EXTERNAL) || (type == SINGULAR_EXTERNAL));
            }

            inline bool is_register_def() const {
                return ((type == REGISTER_EXTERNAL) || (type == REGISTER_INTERNAL));
            }

        private:
            std::variant<Register_resource_definition::Register_qubit_definition, Singular_resource_definition::Singular_qubit_definition> value;
            Type type;
    
    };

    class Bit_definition : public Node {

        public:

            /// @brief Dummy definition
            Bit_definition(): 
                Node("bit_def", hash_rule_name("bit_def")),
                value(Register_resource_definition::Register_bit_definition()), 
                type(REGISTER_EXTERNAL)
            {}

            Bit_definition(Register_resource_definition::Register_bit_definition def, bool external):
                Node("bit_def", hash_rule_name("bit_def")),
                value(def), 
                type(external ? REGISTER_EXTERNAL : REGISTER_INTERNAL)
            {
                if (external) {
                    constraint = std::make_optional<Node_constraint>(Common::register_bit_def_external, 1);
                } else {
                    constraint = std::make_optional<Node_constraint>(Common::register_bit_def_internal, 1);
                }
            }

            Bit_definition(Singular_resource_definition::Singular_bit_definition def, bool external):
                Node("bit_def", hash_rule_name("bit_def")),
                value(def), 
                type(external ? SINGULAR_EXTERNAL : SINGULAR_INTERNAL)
            {
                if (external) {
                    constraint = std::make_optional<Node_constraint>(Common::singular_bit_def_external, 1);
                } else {
                    constraint = std::make_optional<Node_constraint>(Common::singular_bit_def_internal, 1);
                }
            }

            Type get_type(){return type;}

            inline std::shared_ptr<Variable> get_name(){
                return std::visit([](auto&& val) -> std::shared_ptr<Variable> {
                    return val.get_name();
                }, value);
            }

            inline std::shared_ptr<Integer> get_size(){                
                if(is_register_def()){
                    return std::get<Register_resource_definition::Register_bit_definition>(value).get_size();   
                }

                ERROR("Singular qubit definitions do not have sizes!");

                return std::make_shared<Integer>();
            }

            //TODO: May not be needed but keep it here for now
            inline bool is_external() const {
                return ((type == REGISTER_EXTERNAL) || (type == SINGULAR_EXTERNAL));
            }

            inline bool is_register_def() const {
                return ((type == REGISTER_EXTERNAL) || (type == REGISTER_INTERNAL));
            }

        private:
            std::variant<Register_resource_definition::Register_bit_definition, Singular_resource_definition::Singular_bit_definition> value;
            Type type;

    };

}

#endif
