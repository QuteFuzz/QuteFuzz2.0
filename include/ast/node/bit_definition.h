#ifndef BIT_DEFINITION_H
#define BIT_DEFINITION_H

#include <register_bit_definition.h>
#include <singular_bit_definition.h>

namespace Bit_definition {

    enum Type {
        REGISTER_INTERNAL,
        SINGULAR_INTERNAL,
        REGISTER_EXTERNAL,
        SINGULAR_EXTERNAL
    };

    class Bit_definition : public Node {

        public:

            /// @brief Dummy definition
            Bit_definition() : 
                Node("bit_def", hash_rule_name("bit_def")),
                value(Register_bit_definition()), 
                type(REGISTER_EXTERNAL)
            {}

            Bit_definition(Register_bit_definition def, bool external) :
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

            Bit_definition(Singular_bit_definition def, bool external) :
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
                    return std::get<Register_bit_definition>(value).get_size();   
                }

                ERROR("Singular bit definitions do not have sizes!");

                return std::make_shared<Integer>();
            }

            inline bool is_external() const {
                return ((type == REGISTER_EXTERNAL) || (type == SINGULAR_EXTERNAL));
            }

            inline bool is_register_def() const {
                return ((type == REGISTER_EXTERNAL) || (type == REGISTER_INTERNAL));
            }

        private:
            std::variant<Register_bit_definition, Singular_bit_definition> value;
            Type type;
    
    };

}

#endif
