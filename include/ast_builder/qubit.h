#ifndef QUBIT_H
#define QUBIT_H

#include <register_qubit.h>
#include <singular_qubit.h>

namespace Qubit {

    enum Type {
        REGISTER,
        SINGULAR
    };

    class Qubit : public Node {
        public:

            Qubit(Register_qubit qubit) :
                Node("qubit", hash_rule_name("qubit")),
                value(qubit),
                type(REGISTER)
            {
                constraint = std::make_optional<Size_constraint>(Common::register_qubit, 1);
            }

            Qubit(Singular_qubit qubit) :
                Node("qubit", hash_rule_name("qubit")),
                value(qubit),
                type(SINGULAR)
            {
                constraint = std::make_optional<Size_constraint>(Common::singular_qubit, 1);
            }

            Type get_type(){
                return type;
            }

            void reset(){
                std::visit([](auto&& val){
                    val.reset();
                }, value);
            }

            bool is_used(){
                return std::visit([](auto&& val) -> bool {
                    return val.is_used();
                }, value);
            }

            void set_used(){
                std::visit([](auto&& val){
                    val.set_used();
                }, value);
            }

            inline std::shared_ptr<Variable> get_name(){
                return std::visit([](auto&& val) -> std::shared_ptr<Variable> {
                    return val.get_name();
                }, value);
            }

            inline std::shared_ptr<Integer> get_index(){
                if(type == REGISTER){
                    return std::get<Register_qubit>(value).get_index();
                }

                ERROR("Singular qubits do not have indices!");

                return std::make_shared<Integer>("ERROR");
            }
            
        private:
            std::variant<Register_qubit, Singular_qubit> value;
            Type type;
    };



}

#endif
