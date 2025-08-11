#ifndef RESOURCE_H
#define RESOURCE_H

#include <gate.h>
#include <register_resource.h>
#include <singular_resource.h>
#include <dag.h>

namespace Resource {

    enum class Scope {
        EXTERNAL = 1 << 0,
        INTERNAL = 1 << 1,
    };

    inline U8 to_u8(Scope s){
        return static_cast<U8>(s);
    }

    inline U8 operator|(Scope lhs, Scope rhs){
        return to_u8(lhs) | to_u8(rhs);
    }

    inline bool is_external(Scope scope){
        return scope == Scope::EXTERNAL;
    }

    enum Classification {
        QUBIT,
        BIT
    };

    class Resource : public Node {
        public:

            /// @brief Dummy resource
            Resource() :
                Node("resource", hash_rule_name("resource")),
                value(Singular_resource()),
                scope(Scope::EXTERNAL)
            {}

            Resource(std::string str, U64 hash, Register_resource resource, Scope _scope) :
                Node(str, hash),
                value(resource),
                scope(_scope)
            {}

            Resource(std::string str, U64 hash, Singular_resource resource, Scope _scope) :
                Node(str, hash),
                value(resource),
                scope(_scope)
            {}

            U8 get_scope(){
                return to_u8(scope);
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
                return scope == Scope::EXTERNAL;
            }

            inline bool is_register_def() const {
                return std::holds_alternative<Register_resource>(value);
            }

            inline std::shared_ptr<Integer> get_index() const {
                if(is_register_def()){
                    return std::get<Register_resource>(value).get_index();
                }

                ERROR("Singular resource do not have indices!");

                return std::make_shared<Integer>();
            }

            std::string resolved_name() const override;
            
        private:
            std::variant<Register_resource, Singular_resource> value;
            Scope scope;

    };

    class Qubit : public Resource {

        public:
            Qubit() : Resource() {}

            Qubit(Register_qubit qubit, Scope scope) :
                Resource("qubit", Common::qubit, qubit, scope)
            {
                constraint = std::make_optional<Node_constraint>(Common::register_qubit, 1);
            }

            Qubit(Singular_qubit qubit, Scope scope) :
                Resource("qubit", Common::qubit, qubit, scope)
            {
                constraint = std::make_optional<Node_constraint>(Common::singular_qubit, 1);
            }

            void extend_flow_path(const std::shared_ptr<Node> node, size_t current_port);

            void extend_dot_string(std::ostringstream& ss) const;

            void add_path_to_dag(Dag::Dag& dag) const;

            std::vector<Dag::Edge> get_flow_path(){
                return flow_path;
            }


        private:
            std::vector<Dag::Edge> flow_path;
            std::string flow_path_colour = random_hex_colour();
            size_t flow_path_length = 0;

    };

    class Bit : public Resource {

        public:
            Bit() : Resource() {}

            Bit(Register_bit bit, Scope scope) :
                Resource("bit", Common::bit, bit, scope)
            {
                constraint = std::make_optional<Node_constraint>(Common::register_bit, 1);
            }

            Bit(Singular_bit bit, Scope scope) :
                Resource("bit", Common::bit, bit, scope)
            {
                constraint = std::make_optional<Node_constraint>(Common::singular_bit, 1);
            }

        private:

    };


}

#define ALL_SCOPES (Resource::Scope::EXTERNAL | Resource::Scope::INTERNAL)


#endif
