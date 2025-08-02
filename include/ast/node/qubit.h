#ifndef QUBIT_H
#define QUBIT_H

#include <gate.h>
#include <register_qubit.h>
#include <singular_qubit.h>
#include <dag.h>

namespace Qubit {

    enum Type {
        REGISTER_EXTERNAL,
        REGISTER_INTERNAL,
        SINGULAR_EXTERNAL,
        SINGULAR_INTERNAL
    };

    class Qubit : public Node {
        public:

            /// @brief Dummy qubit
            Qubit() :
                Node("qubit", hash_rule_name("qubit")),
                value(Singular_qubit()),
                type(SINGULAR_EXTERNAL)
            {}

            Qubit(Register_qubit qubit, bool external) :
                Node("qubit", hash_rule_name("qubit")),
                value(qubit),
                type(external ? REGISTER_EXTERNAL : REGISTER_INTERNAL)
            {
                constraint = std::make_optional<Node_constraint>(Common::register_qubit, 1);
            }

            Qubit(Singular_qubit qubit, bool external) :
                Node("qubit", hash_rule_name("qubit")),
                value(qubit),
                type(external ? SINGULAR_EXTERNAL : SINGULAR_INTERNAL)
            {
                constraint = std::make_optional<Node_constraint>(Common::singular_qubit, 1);
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

            inline std::shared_ptr<Variable> get_name() const {
                return std::visit([](auto&& val) -> std::shared_ptr<Variable> {
                    return val.get_name();
                }, value);
            }

            inline bool is_external() const {
                return ((type == REGISTER_EXTERNAL) || (type == SINGULAR_EXTERNAL));
            }

            inline bool is_register_def() const {
                return ((type == REGISTER_EXTERNAL) || (type == REGISTER_INTERNAL));
            }

            inline std::shared_ptr<Integer> get_index() const {
                if(is_register_def()){
                    return std::get<Register_qubit>(value).get_index();
                }

                ERROR("Singular qubits do not have indices!");

                return std::make_shared<Integer>();
            }

            std::vector<Dag::Edge> get_flow_path(){
                return flow_path;
            }

            std::string resolved_name() const override;
            
            void extend_flow_path(const std::shared_ptr<Node> node, size_t current_port);

            void extend_dot_string(std::ostringstream& ss) const;

            void add_path_to_adj_list(std::unordered_map<int, std::vector<int>>& adj_list) const;
            
        private:
            std::variant<Register_qubit, Singular_qubit> value;
            Type type;

            std::vector<Dag::Edge> flow_path;
            std::string flow_path_colour = random_hex_colour();
            size_t flow_path_length = 0;
    };

}

#endif
