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

        void prepare_node(std::shared_ptr<Node> node);

        void write_branch(std::shared_ptr<Node> node);

        std::optional<std::pair<int, U64>> initiator_amount(U64 hash, int num_completer = WILDCARD_MAX){
            if(hash == Common::qreg_defs){
                auto res = std::make_pair<int, U64>(Common::setup_qregs(get_qreg_defs(), num_completer), Common::qreg_def);
                return std::optional<std::pair<int, U64>>(res);

            } else if (hash == Common::subroutines){
                auto res = std::make_pair<int, U64>(get_amount(num_completer, 0, Common::MAX_SUBROUTINES), Common::circuit);
                return std::optional<std::pair<int, U64>>(res);

            } else {
                return std::nullopt;
            }
        }

        /// if node that would normally depend on another node for its setup isn't defined as an initiator, use this to set it up instead
        void inline initiator_default_setup(U64 hash){
            if(!node_deps.node_is(ND_INIT, hash)){
                std::optional<std::pair<int, U64>> amount = initiator_amount(hash);

                if(amount.has_value()){
                    constraints.add_rules_constraint(hash, Constraints::NUM_GIVEN_RULE_EQUALS, amount.value().first, amount.value().second);
                }
            }
        }

        /// @brief Check whether current circuit can supply qubits to at least one of the defined subroutines that aren't itself
        /// @return 
        bool can_apply_subroutines(){
            
            for(const auto& qreg_defs : all_qreg_defs){
                if(!qreg_defs->owned_by(Common::TOP_LEVEL_CIRCUIT_NAME) && !qreg_defs->owned_by(current_circuit_node->get_circuit_name())){
                    return get_qreg_defs()->num_qubits() >= qreg_defs->num_qubits();
                }
            }

            return false;
        }


        /// @brief This will only be called when it is known that at least one subroutine was generated. can safely loop until a subrountine is found
        /// that isn't the current subroutine
        std::shared_ptr<Common::Qreg_definitions> get_random_subroutine(){
            int random_index = random_int(all_qreg_defs.size() - 1);
            std::shared_ptr<Common::Qreg_definitions> ret = all_qreg_defs[random_index];

            while(ret->owned_by(Common::TOP_LEVEL_CIRCUIT_NAME) || ret->owned_by(current_circuit_node->get_circuit_name())){
                random_index = random_int(all_qreg_defs.size() - 1);
                ret = all_qreg_defs[random_index];
            }

            return ret;
        }

        /// @brief Get qreg defs of current circuit
        /// @return 
        std::shared_ptr<Common::Qreg_definitions> get_qreg_defs(){
            std::string circuit_name = current_circuit_node->get_circuit_name();

            for(const auto& qreg_defs : all_qreg_defs){
                if(qreg_defs->owned_by(circuit_name)){
                    return qreg_defs;
                }
            }

            std::shared_ptr<Common::Qreg_definitions> ptr = std::make_shared<Common::Qreg_definitions>(circuit_name);
            all_qreg_defs.push_back(ptr);

            return ptr;
        }

        Result<Node, std::string> build(){
            Result<Node, std::string> res;

            node_deps = main_circ_deps.value_or(node_deps);
            node_deps.reset();
            all_qreg_defs.clear();
        
            if(entry == nullptr){
                res.set_error("Entry point not set");
                return res;
        
            } else {
                builds += 1;

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

        int recursions = 5;
        int builds = 0;
        std::shared_ptr<Rule> entry = nullptr;

        std::shared_ptr<Node> root_ptr = nullptr;

        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<float> float_dist;

        std::vector<std::shared_ptr<Common::Qreg_definitions>> all_qreg_defs;
        std::shared_ptr<Common::Qreg> qreg_to_write = Common::DEFAULT_QREG;
        std::shared_ptr<Common::Qubit> qubit_to_write = Common::DEFAULT_QUBIT;
        
        std::shared_ptr<Node> subs_node = nullptr;
        std::shared_ptr<Node> current_circuit_node = nullptr;
        int current_subroutine = 0;

        Constraints::Constraints constraints;

        Node_dependencies node_deps;
        std::optional<Node_dependencies> main_circ_deps; 
};

#endif
