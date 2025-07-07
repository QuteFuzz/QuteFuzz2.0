#ifndef NODE_H
#define NODE_H

#include <utils.h>
#include <branch.h>

enum Node_build_state {
    NB_DONE,
    NB_BUILD,
};


enum Node_kind {
    TERMINAL,
    NON_TERMINAL,
};


struct Size_constraint {

    public:
        Size_constraint(Common::Rule_hash _rule, size_t _occurances): 
            rule(_rule),
            occurances(_occurances)
        {}

        bool passed(const Branch& branch){
            return branch.count_rule_occurances(rule) == occurances;
        }

    private:
        Common::Rule_hash rule;
        size_t occurances = 0;

};


/// @brief A node is a term with pointers to other nodes
class Node {

    public:
        Node(){}

        Node(const std::string _string, const U64 _hash = 0ULL):
            string(_string),
            hash(_hash),
            kind((hash == 0ULL) ? TERMINAL : NON_TERMINAL)
        {}

        void add_child(const std::shared_ptr<Node> child){
            child->set_depth(depth+1);
            children.push_back(child);
        }

        void transition_to_done(){
            state = NB_DONE;
        }

        Node_build_state build_state(){
            return state;
        }

        void extend_str(std::string& str, std::string repeat, std::string end = "") const {
            for(int i = 0; i < depth; ++i){
                str += repeat;
            }

            str += end;
        }

        /// @brief Indent for ast printing only
        std::string indent() const {
            std::string str = "\n";

            extend_str(str, "\t", "|\n");

            extend_str(str, "\t", "|");

            extend_str(str, "---", ">");

            return str;
        }

        std::string get_string() const {
            return string;
        }

        Node_kind get_node_kind() const {return kind;}

        friend std::ostream& operator<<(std::ostream& stream, const Node& n) {
            stream << "[" << n.string << "]" << " children: " << n.get_num_children();
            
            for(auto child : n.children){
                stream << child->indent() << *child;
            }

            return stream;
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

        size_t get_num_children() const {
            return children.size();
        }

        int get_depth() const {
            return depth;
        }

        void set_depth(int _depth){
            depth = _depth;
        }

        bool operator==(const U64& other){
            return hash == other;
        }

        bool operator==(const std::string& other){
            return string == other;
        }

        bool branch_satisfies_constraint(const Branch& branch){
            return !constraint.has_value() || constraint.value().passed(branch);
        }

        void make_dot_string(std::string& ret) const {
            for(const std::shared_ptr<Node>& child : children){                
                ret += "    " + escape(string) + " -> " + escape(child->get_string()) + ";\n";
                child->make_dot_string(ret);                   
            }
        }

    protected:
        std::string string;
        U64 hash;
        Node_kind kind;

        int depth = 0;

        std::vector<std::shared_ptr<Node>> children;

        Node_build_state state = NB_BUILD;

        std::optional<Size_constraint> constraint;
};

#endif
