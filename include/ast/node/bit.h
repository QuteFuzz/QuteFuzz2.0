#ifndef BIT_H
#define BIT_H

#include <gate.h>
#include <register_bit.h>
#include <singular_bit.h>
#include <dag.h>

namespace Bit {

    enum Type {
        REGISTER_EXTERNAL,
        REGISTER_INTERNAL,
        SINGULAR_EXTERNAL,
        SINGULAR_INTERNAL
    };

    class Bit : public Node {
        public:

            /// @brief Dummy bit
            Bit() :
                Node("bit", hash_rule_name("bit")),
                value(Singular_bit()),
                type(SINGULAR_EXTERNAL)
            {}

            Bit(Register_bit bit, bool external) :
                Node("bit", hash_rule_name("bit")),
                value(bit),
                type(external ? REGISTER_EXTERNAL : REGISTER_INTERNAL)
            {
                constraint = std::make_optional<Node_constraint>(Common::register_bit, 1);
            }

            Bit(Singular_bit bit, bool external) :
                Node("bit", hash_rule_name("bit")),
                value(bit),
                type(external ? SINGULAR_EXTERNAL : SINGULAR_INTERNAL)
            {
                constraint = std::make_optional<Node_constraint>(Common::singular_bit, 1);
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
                    return std::get<Register_bit>(value).get_index();
                }

                ERROR("Singular bits do not have indices!");

                return std::make_shared<Integer>();
            }

            std::vector<Dag::Edge> get_flow_path(){
                return flow_path;
            }

            std::string resolved_name() const override;
            
            void extend_flow_path(const std::shared_ptr<Node> node, size_t current_port);

            void extend_dot_string(std::ostringstream& ss) const;

            void add_path_to_heuristics(Dag::Heuristics& h) const;
            
        private:
            std::variant<Register_bit, Singular_bit> value;
            Type type;

            std::vector<Dag::Edge> flow_path;
            std::string flow_path_colour = random_hex_colour();
            size_t flow_path_length = 0;
    };

}

#endif
