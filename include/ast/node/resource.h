#ifndef RESOURCE_H
#define RESOURCE_H

#include <gate.h>
#include <register_resource.h>
#include <singular_resource.h>
#include <dag.h>

namespace Resource {

    enum Resource_Type {
        REGISTER_EXTERNAL,
        REGISTER_INTERNAL,
        SINGULAR_EXTERNAL,
        SINGULAR_INTERNAL
    };

    enum Resource_Classification {
        QUBIT,
        BIT
    };

    class Resource : public Node {
        public:

            /// @brief Dummy resource
            Resource() :
                Node("resource", hash_rule_name("resource")),
                value(Singular_resource()),
                type(SINGULAR_EXTERNAL),
                resource_type(QUBIT)
            {}

            Resource(Register_resource resource, bool external) :
                Node(resource.get_resource_classification() ? "qubit" : "bit", 
                    hash_rule_name(resource.get_resource_classification() ? "qubit" : "bit")),
                value(resource),
                type(external ? REGISTER_EXTERNAL : REGISTER_INTERNAL),
                resource_type(resource.get_resource_classification() ? QUBIT : BIT)
            {
                if (resource.get_resource_classification()) {
                    constraint = std::make_optional<Node_constraint>(Common::register_qubit, 1);
                } else {
                    constraint = std::make_optional<Node_constraint>(Common::register_bit, 1);
                }
            }

            Resource(Singular_resource resource, bool external) :
                Node(resource.get_resource_classification() ? "qubit" : "bit", 
                    hash_rule_name(resource.get_resource_classification() ? "qubit" : "bit")),
                value(resource),
                type(external ? SINGULAR_EXTERNAL : SINGULAR_INTERNAL),
                resource_type(resource.get_resource_classification() ? QUBIT : BIT)
            {
                if (resource.get_resource_classification()) {
                    constraint = std::make_optional<Node_constraint>(Common::singular_qubit, 1);
                } else {
                    constraint = std::make_optional<Node_constraint>(Common::singular_bit, 1);
                }
            }

            Resource_Type get_type(){
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
                    return std::get<Register_resource>(value).get_index();
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
            std::variant<Register_resource, Singular_resource> value;
            Resource_Type type;
            Resource_Classification resource_type;

            std::vector<Dag::Edge> flow_path;
            std::string flow_path_colour = random_hex_colour();
            size_t flow_path_length = 0;
    };

}

#endif
