#ifndef QUBIT_DEFINITION_H
#define QUBIT_DEFINITION_H

#include <register_qubit_definition.h>
#include <singular_qubit_definition.h>

namespace Qubit_definition {

    enum Type {
        REGISTER,
        SINGULAR
    };

    class Qubit_definition : public Node {

        public:

            Qubit_definition() : 
                Node("qubit_def", hash_rule_name("qubit_def")),
                value(Register_qubit_definition()), 
                type(REGISTER)
            {}

            Qubit_definition(Register_qubit_definition def) :
                Node("qubit_def", hash_rule_name("qubit_def")),
                value(def), 
                type(REGISTER)
            {
                constraint = std::make_optional<Size_constraint>(Common::register_qubit_def, 1);
            }

            Qubit_definition(Singular_qubit_definition def) :
                Node("qubit_def", hash_rule_name("qubit_def")),
                value(def), 
                type(SINGULAR)
            {
                constraint = std::make_optional<Size_constraint>(Common::singular_qubit_def, 1);
            }

            Type get_type(){return type;}

            inline std::shared_ptr<Variable> get_name(){
                return std::visit([](auto&& val) -> std::shared_ptr<Variable> {
                    return val.get_name();
                }, value);
            }

            inline std::shared_ptr<Integer> get_size(){
                if(type == REGISTER){
                    return std::get<Register_qubit_definition>(value).get_size();
                }

                ERROR("Singular qubit definitions do not have sizes!");

                return std::make_shared<Integer>();
            }

        private:
            std::variant<Register_qubit_definition, Singular_qubit_definition> value;
            Type type;
    
    };

}

#endif
