#ifndef RESOURCE_H
#define RESOURCE_H

#include <gate.h>
#include <register_resource.h>
#include <singular_resource.h>
#include <dag.h>

namespace Resource {

    #define EXTERNAL_SCOPE (1 << 0)
    #define INTERNAL_SCOPE (1 << 1)
    #define OWNED_SCOPE (1 << 2)
    #define ALL_SCOPES (EXTERNAL_SCOPE | INTERNAL_SCOPE | OWNED_SCOPE)

    inline bool is_external(U8 scope){
        return scope & EXTERNAL_SCOPE;
    }

    inline bool is_owned(U8 scope){
        return scope & OWNED_SCOPE;
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
                scope(EXTERNAL_SCOPE)
            {}

            Resource(std::string str, U64 hash, const Register_resource& resource, const U8& _scope) :
                Node(str, hash),
                value(resource),
                scope(_scope)
            {}

            Resource(std::string str, U64 hash, const Singular_resource& resource, const U8& _scope) :
                Node(str, hash),
                value(resource),
                scope(_scope)
            {}

            U8 get_scope() const {
                return scope;
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
                return scope & EXTERNAL_SCOPE;
            }

            inline bool is_owned() const {
                return scope & OWNED_SCOPE;
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
            U8 scope;

    };

    class Qubit : public Resource {

        public:
            Qubit() : Resource() {}

            Qubit(const Register_qubit& qubit, const U8& scope) :
                Resource("qubit", Common::qubit, qubit, scope)
            {
                constraint = std::make_optional<Node_constraint>(Common::register_qubit, 1);
            }

            Qubit(const Singular_qubit& qubit, const U8& scope) :
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

            Bit(const Register_bit& bit, const U8& scope) :
                Resource("bit", Common::bit, bit, scope)
            {
                constraint = std::make_optional<Node_constraint>(Common::register_bit, 1);
            }

            Bit(const Singular_bit& bit, const U8& scope) :
                Resource("bit", Common::bit, bit, scope)
            {
                constraint = std::make_optional<Node_constraint>(Common::singular_bit, 1);
            }

        private:

    };


}




#endif
