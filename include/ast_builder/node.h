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

        /// no hash provided => node is non-terminal
        Node(const std::string _string, const U64 _hash = 0ULL, const std::string _indentation_str = ""):
            string(_string),
            hash(_hash),
            kind((hash == 0ULL) ? TERMINAL : NON_TERMINAL),
            indentation_str(_indentation_str)
        {}

        void add_child(const std::shared_ptr<Node> child){
            children.push_back(child);
        }

        void transition_to_done(){
            state = NB_DONE;
        }

        Node_build_state build_state(){
            return state;
        }

        std::string get_string() const {
            return string;
        }

        Node_kind get_node_kind() const {return kind;}

        friend std::ostream& operator<<(std::ostream& stream, const Node& n) {

            if(n.kind == TERMINAL){
                stream << n.string;

            } else {

                for(const std::shared_ptr<Node>& child : n.children){
                    stream << n.indentation_str << *child;
                }
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

        static std::string indentation_tracker;

    protected:
        std::string string;
        U64 hash;
        Node_kind kind;

        std::string indentation_str;

        std::vector<std::shared_ptr<Node>> children;

        Node_build_state state = NB_BUILD;

        std::optional<Size_constraint> constraint;
};

#endif
