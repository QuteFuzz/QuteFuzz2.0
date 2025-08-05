#ifndef RESOURCE_H
#define RESOURCE_H

#include <gate.h>
#include <register_resource.h>
#include <singular_resource.h>
#include <dag.h>

namespace Resource {

    enum Type {
        REGISTER_EXTERNAL,
        REGISTER_INTERNAL,
        SINGULAR_EXTERNAL,
        SINGULAR_INTERNAL
    };

    enum Resource_Type {
        QUBIT,
        BIT
    };

    class Resource : public Node {
        public:

            /// @brief Dummy qubit
            Resource() :
                Node("resource", hash_rule_name("resource")),
                value(Singular_resource::Singular_qubit()),
                type(SINGULAR_EXTERNAL),
                resource_type(QUBIT)
            {}

            Resource(Register_resource::Register_qubit qubit, bool external) :
                Node("qubit", hash_rule_name("qubit")),
                value(qubit),
                type(external ? REGISTER_EXTERNAL : REGISTER_INTERNAL),
                resource_type(QUBIT)
            {
                constraint = std::make_optional<Node_constraint>(Common::register_qubit, 1);
            }

            Resource(Singular_resource::Singular_qubit qubit, bool external) :
                Node("qubit", hash_rule_name("qubit")),
                value(qubit),
                type(external ? SINGULAR_EXTERNAL : SINGULAR_INTERNAL),
                resource_type(BIT)
            {
                constraint = std::make_optional<Node_constraint>(Common::singular_qubit, 1);
            }

            Resource(Register_resource::Register_bit bit, bool external) :
                Node("bit", hash_rule_name("bit")),
                value(bit),
                type(external ? REGISTER_EXTERNAL : REGISTER_INTERNAL),
                resource_type(BIT)
            {
                constraint = std::make_optional<Node_constraint>(Common::register_bit, 1);
            }

            Resource(Singular_resource::Singular_bit bit, bool external) :
                Node("bit", hash_rule_name("bit")),
                value(bit),
                type(external ? SINGULAR_EXTERNAL : SINGULAR_INTERNAL),
                resource_type(BIT)
            {
                constraint = std::make_optional<Node_constraint>(Common::singular_bit, 1);
            }

            Type get_type(){
                return type;
            }

            Resource_Type get_resource_type(){
                return resource_type;
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

            inline bool is_qubit() const {
                return ((resource_type == QUBIT));
            }

            inline bool is_external() const {
                return ((type == REGISTER_EXTERNAL) || (type == SINGULAR_EXTERNAL));
            }

            inline bool is_register_def() const {
                return ((type == REGISTER_EXTERNAL) || (type == REGISTER_INTERNAL));
            }

            inline std::shared_ptr<Integer> get_index() const {
                if(is_register_def()){
                    if (is_qubit()) return std::get<Register_resource::Register_qubit>(value).get_index();
                    else return std::get<Register_resource::Register_bit>(value).get_index();
                }

                ERROR("Singular resource do not have indices!");

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
            std::variant<Register_resource::Register_qubit, Singular_resource::Singular_qubit, Register_resource::Register_bit, Singular_resource::Singular_bit> value;
            Type type;
            Resource_Type resource_type;

            std::vector<Dag::Edge> flow_path;
            std::string flow_path_colour = random_hex_colour();
            size_t flow_path_length = 0;
    };

}

#endif
