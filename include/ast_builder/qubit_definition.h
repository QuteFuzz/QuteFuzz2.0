#ifndef QUBIT_DEFINITION_H
#define QUBIT_DEFINITION_H

#include <register_qubit_definition.h>
#include <singular_qubit_definition.h>

namespace Qubit_definition {

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
                value(Register_qubit_definition()), 
                type(REGISTER_EXTERNAL)
            {}

            Qubit_definition(Register_qubit_definition def, bool external) :
                Node("qubit_def", hash_rule_name("qubit_def")),
                value(def), 
                type(external ? REGISTER_EXTERNAL : REGISTER_INTERNAL)
            {
                if (external) {
                    constraint = std::make_optional<Size_constraint>(Common::register_qubit_def_external, 1);
                } else {
                    constraint = std::make_optional<Size_constraint>(Common::register_qubit_def_internal, 1);
                }
            }

            Qubit_definition(Singular_qubit_definition def, bool external) :
                Node("qubit_def", hash_rule_name("qubit_def")),
                value(def), 
                type(external ? SINGULAR_EXTERNAL : SINGULAR_INTERNAL)
            {
                if (external) {
                    constraint = std::make_optional<Size_constraint>(Common::singular_qubit_def_external, 1);
                } else {
                    constraint = std::make_optional<Size_constraint>(Common::singular_qubit_def_internal, 1);
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
                    return std::get<Register_qubit_definition>(value).get_size();   
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
            std::variant<Register_qubit_definition, Singular_qubit_definition> value;
            Type type;
    
    };

}

#endif
