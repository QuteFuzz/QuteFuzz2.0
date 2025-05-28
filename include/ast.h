#ifndef AST_H
#define AST_H

#include <optional>
#include "term.h"
#include "grammar.h"
#include "constraints.h"
#include "dependencies.h"
#include "node.h"

class Ast{
    public:
        Ast() : gen(rd()), float_dist(0.0, 1.0), node_deps(Common::gate_application) {
            node_deps.add_initiator(Common::qreg_defs);
            node_deps.add_initiator(Common::subroutines);
        }

        ~Ast() = default;

        void set_entry(const std::shared_ptr<Rule> _entry){
            entry = _entry;
        }   

        Result<Branch, std::string> pick_branch(const std::shared_ptr<Rule> rule);

        void add_dependency(U64 init, U64 comp){
            if(node_deps.node_is(ND_COMP, comp))
                node_deps.add_initiator(init);
        }

        void resolve_dependency(std::shared_ptr<Node> initiator_node, int dependency_info);

        Node_build_state transition_from_ready(std::shared_ptr<Node> node, Branch& chosen_branch);

        Node_build_state transition_from_init(std::shared_ptr<Node> node);

        Node_build_state transition_from_stall(std::shared_ptr<Node> node);

        void add_constraint(std::shared_ptr<Node> node);

        void write_branch(std::shared_ptr<Node> node);

        std::optional<int> initiator_amount(U64 hash, int num_completer = WILDCARD_MAX){
            if(hash == Common::qreg_defs){
                return Common::setup_qregs(qreg_defs, random_int(num_completer));

            } else if (hash == Common::subroutines){
                return get_amount(random_int(num_completer), Common::MIN_SUBROUTINES, Common::MAX_SUBROUTINES);

            } else {
                return std::nullopt;
            }
        }

        /// if node that would normally depend on another node for its setup isn't defined as an initiator, use this to set it up instead
        bool inline initiator_default_setup(U64 hash){
            std::optional<int> amount = initiator_amount(hash);

            if(!node_deps.node_is(ND_INIT, hash) && amount.has_value()){
                constraints.add_rules_constraint(hash, Constraints::NUM_RULES_EQUALS, amount.value());
                return true;
            }

            return false;
        }

        Result<Node, std::string> build(){
            Result<Node, std::string> res;

            node_deps = main_circ_deps.value_or(node_deps);
            node_deps.reset();
        
            if(entry == nullptr){
                res.set_error("Entry point not set");
                return res;
        
            } else {
                root_ptr = std::make_shared<Node>(entry, 0);

                write_branch(root_ptr);

                res.set_ok(*root_ptr);
                return res;
            }
        }

        /// @brief Try to find particular node by searching from root pointer
        /// @param hash 
        /// @return 
        std::shared_ptr<const Node> find(Common::Rule_hash hash){            
            return root_ptr->find(hash);
        }

        virtual void ast_to_program(fs::path& path);

    protected:
        virtual std::string imports(){
            return "";
        }

        virtual std::string compiler_call(){
            return "";
        }

        /// @brief Simplest writer simply prints all terminals, or loops through all children until it eaches a terminal
        /// @param stream 
        /// @param node 
        /// @return 
        virtual std::ofstream& write(std::ofstream& stream, const Node& node) {
            std::string str = node.get_string();

            if(node.is_syntax()){
                stream << str;            
                return stream;    
            } else {
                write_children(stream, node);
            }
        
            return stream;
        };

        /// @brief Loop through and call `write` on each child of the given node
        /// @param stream 
        /// @param node
        /// @param end_string a string to write at the end of each call to `write` (Optional)
        /// @return 
        std::ofstream& write_children(std::ofstream& stream, const Node& node, const std::string end_string = ""){
            std::vector<std::shared_ptr<Node>> children = node.get_children();

            for(auto child : children){
                write(stream, *child) << end_string;
            }

            return stream;
        }

        Constraints::Constraints default_constraints;

        int recursions = 5;
        std::shared_ptr<Rule> entry = nullptr;

        std::shared_ptr<Node> root_ptr = nullptr;

        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<float> float_dist;

        Common::Qreg_definitions qreg_defs;
        std::shared_ptr<Common::Qreg> qreg_to_write = Common::DEFAULT_QREG;
        std::shared_ptr<Common::Qubit> qubit_to_write = Common::DEFAULT_QUBIT;
        std::string circuit_name = Common::TOP_LEVEL_CIRCUIT_NAME;
        
        std::shared_ptr<Node> subs_node = nullptr;
        int current_subroutine = 0;

        Constraints::Constraints constraints;

        Node_dependencies node_deps;
        std::optional<Node_dependencies> main_circ_deps; 
};

#endif
