#ifndef AST_H
#define AST_H

#include <optional>
#include <algorithm>
#include "term.h"
#include "grammar.h"
#include "node.h"
#include "constraints.h"
#include "graph.h"
#include "qubit.h"

class Ast{
    public:
        Ast(fs::path _output_dir) : gen(rd()), float_dist(0.0, 1.0), output_dir(_output_dir) {}

        ~Ast() = default;

        int num_circuits = 0;

        void set_entry(const std::shared_ptr<Rule> _entry){
            entry = _entry;
        }   

        Result<Branch, std::string> pick_branch(const std::shared_ptr<Rule> rule);

        void transition_from_ready(std::shared_ptr<Node> node);

        void transition_from_init(std::shared_ptr<Node> node);

        void prepare_node(std::shared_ptr<Node> node);

        void write_branch(std::shared_ptr<Node> node);

        /// @brief Check whether current circuit can supply qubits to at least one of the defined subroutines that aren't itself
        /// @return 
        bool can_apply_subroutines(){
            std::shared_ptr<Qreg_definitions> current_qreg_defs = all_qreg_defs[current_circuit_name];
            
            for(const auto& pair : all_qreg_defs){
                if((pair.first != Common::TOP_LEVEL_CIRCUIT_NAME) && (pair.first != current_circuit_name) && (current_qreg_defs->num_qubits() >= pair.second->num_qubits())){
                    return true;
                }
            }

            return false;
        }


        /// @brief This will only be called when it is known that at least one subroutine was generated which uses less qubits than those defined in 
        /// the circuit currently being generated
        /// can safely loop until a subrountine is found
        std::shared_ptr<Qreg_definitions> get_random_subroutine(){            
            std::unordered_map<std::string, std::shared_ptr<Qreg_definitions>>::iterator it = all_qreg_defs.begin();
            std::advance(it, random_int(all_qreg_defs.size() - 1));
            std::shared_ptr<Qreg_definitions> ret = it->second;

            while(ret->owned_by(Common::TOP_LEVEL_CIRCUIT_NAME) || ret->owned_by(current_circuit_name) || (ret->num_qubits() > get_current_qreg_defs()->num_qubits())){                
                it = all_qreg_defs.begin();
                std::advance(it, random_int(all_qreg_defs.size() - 1));
                ret = it->second;
            }

            return ret;
        }

        /// @brief Get qreg defs of current circuit if already in map. If not, create entry and return pointer
        /// @return 
        std::shared_ptr<Qreg_definitions> get_current_qreg_defs(){
            if(all_qreg_defs.find(current_circuit_name) == all_qreg_defs.end()){
                std::shared_ptr<Qreg_definitions> ptr = std::make_shared<Qreg_definitions>(current_circuit_name);
                all_qreg_defs[current_circuit_name] = ptr;
            }

            return all_qreg_defs[current_circuit_name];
        }

        int get_max_defined_qubits(){
            int res = Common::MIN_QUBITS;

            for(const auto& pair : all_qreg_defs){
                res = std::max(res, (int)pair.second->num_qubits());
            }

            return res;
        }

        bool in_subroutine(){
            return (subs_node != nullptr) && (subs_node->build_state() == NB_READY);
        }

        Result<Node, std::string> build(){
            Result<Node, std::string> res;
        
            if(entry == nullptr){
                res.set_error("Entry point not set");
                return res;
        
            } else {
                all_qreg_defs.clear();

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
        
        std::shared_ptr<Rule> entry = nullptr;
        std::shared_ptr<Node> root_ptr = nullptr;

        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<float> float_dist;

        std::unordered_map<std::string, std::shared_ptr<Qreg_definitions>> all_qreg_defs;
        std::shared_ptr<Qreg> qreg_to_write = DEFAULT_QREG;
        std::shared_ptr<Qubit> qubit_to_write = DEFAULT_QUBIT;
        
        std::shared_ptr<Node> subs_node = nullptr;
        int current_subroutine = 0;
        std::string current_circuit_name;

        Constraints::Constraints constraints;
        
        std::unique_ptr<Graph> qig = nullptr;
        std::optional<std::vector<int>> best_entanglement = std::nullopt;

        fs::path output_dir;
};

#endif
