#ifndef NODE_H
#define NODE_H

#include "term.h"
#include "grammar.h"

enum Node_build_state {
    NB_INIT,
    NB_READY,
    NB_DONE,
    NB_STALL,
};

/// @brief A node is a term with pointers to other nodes
class Node : public std::enable_shared_from_this<Node> {

    public:
        Node(){}

        /// @brief Create node from a term. 
        /// @param _term 
        /// @param _depth 
        Node(const Term& _term, int _depth) : term(_term), depth(_depth){}

        Node(const std::string syntax){
            term.set(syntax);
            nb = NB_DONE;
        }
        
        /// @brief Used to create the AST root from the entry point into the grammar
        /// @param rule 
        /// @param _depth 
        Node(const std::shared_ptr<Rule> rule, int _depth) {
            term.set(rule);
            depth = _depth;
        }
        
        void add_child(const std::shared_ptr<Node> child){
            child->set_depth(depth+1);
            children.push_back(child);

            if(child->is_rule_pointer()) num_rule_pointer_nodes++;
        }

        void extend_str(std::string& str, std::string repeat, std::string end = "") const {
            for(int i = 0; i < depth; ++i){
                str += repeat;
            }

            str += end;
        }

        std::string indent() const {
            std::string str = "\n";

            extend_str(str, "\t", "|\n");

            extend_str(str, "\t", "|");

            extend_str(str, "---", ">");

            return str;
        }

        U64 get_hash() const {
            return term.get_hash();
        }

        Term get_term() const {
            return term;
        }

        void set_circuit_name(std::string name){
            circuit_name = name;
        }

        std::string get_circuit_name(){
            return circuit_name;
        }

        std::string get_string() const {
            return term.get_string();
        }

        bool is_syntax() const {
            return term.is_syntax();
        }

        bool is_rule_pointer() const {
            return term.is_rule_pointer();
        }

        friend std::ostream& operator<<(std::ostream& stream, const Node& n) {
            stream << "[" << n.term << "]" << " children: " << n.get_num_children()
                << " state: ";

            if(n.nb == NB_STALL){
                stream << "STALL";

            } else if (n.nb == NB_DONE){
                stream << "DONE";

            } else if (n.nb == NB_READY){
                stream << "READY";
            
            } else if (n.nb == NB_INIT){
                stream << "INIT";
            } 

            for(auto child : n.children){
                stream << child->indent() << *child;
            }

            return stream;
        }

        std::shared_ptr<const Node> find(Common::Rule_hash hash) const {
            if(term.get_hash() == hash){
                return shared_from_this();
            }
           
            for(auto child : children){
                auto node = child->find(hash);
                if(node != nullptr){
                    return node;
                }
            }

            return nullptr;
        }

        /// @brief Count number of occurances of given node under this node
        /// @param hash 
        /// @return 
        int count(Common::Rule_hash hash) const {
            if(term.get_hash() == hash){
                return 1;
            }

            int ret = 0;
           
            for(auto child : children){
                ret += child->count(hash);
            }

            return ret;
        }

        std::vector<std::shared_ptr<Node>> get_children() const {
            return children;
        }

        std::shared_ptr<Node> child_at(size_t index) const {
            if(index < children.size()){
                return children.at(index);
            } else {
                return nullptr;
            }
        }

        int get_children_in_state(Node_build_state nb) const {
            int count = 0;

            for(const auto& child : children){
                if(child->build_state() == nb){
                    count ++;
                }
            }

            return count;
        }

        bool children_built() const {
            if(children.size()){
                for(const auto& child : children){
                    if(child->build_state() != NB_DONE){
                        return false;
                    }
                }

                return true;
            } else {
                return false;
            }
        }

        size_t get_num_children() const {
            return children.size();
        }

        size_t get_num_rule_pointers() const {
            return num_rule_pointer_nodes;
        }

        int get_depth() const {
            return depth;
        }

        void set_depth(int _depth){
            depth = _depth;
        }

        Node_build_state build_state() const {
            return nb;
        }

        /// @brief If setting build state to stall, must also state which initialiser the node is stalling for
        /// @param _nb 
        /// @param _stalling_for 
        void set_build_state(Node_build_state _nb){
            nb = _nb;
        }

        /// @brief Get branch that was chosen while node was in stall state
        /// @return chosen_branch
        Branch get_branch() const {
            return chosen_branch;
        }

        /// @brief Save branch while in stall state
        /// @param branch 
        void save_branch(const Branch& branch){
            chosen_branch = branch;
            chosen_branch_flag = true;
        }

        bool operator==(const U64& hash){
            return term == hash;
        }

        bool has_chosen_branch(){return chosen_branch_flag;}

    private:
        Term term;
        int depth = 0;
        std::vector<std::shared_ptr<Node>> children;
        size_t num_rule_pointer_nodes = 0;
        Node_build_state nb = NB_INIT;
        Branch chosen_branch;
        bool chosen_branch_flag = false;
        std::string circuit_name;

};

#endif
