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


struct Node_constraint {

    public:
        Node_constraint(Common::Rule_hash _rule, size_t _occurances): 
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
        static std::string indentation_tracker;
        static int node_counter;

        Node(){}

        /// no hash provided => node is non-terminal
        Node(const std::string _string, const U64 _hash = 0ULL, const std::string _indentation_str = ""):
            string(_string),
            hash(_hash),
            kind((hash == 0ULL) ? TERMINAL : NON_TERMINAL),
            indentation_str(_indentation_str)
        {
            id = node_counter++;
        }

        Node(const std::string _string, const U64 _hash, const Node_constraint& _constraint, const std::string _indentation_str = ""):
            string(_string),
            hash(_hash),
            kind((hash == 0ULL) ? TERMINAL : NON_TERMINAL),
            indentation_str(_indentation_str),
            constraint(_constraint)
        {}

        virtual ~Node() = default;

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

        int get_id() const {
            return id;
        }

        virtual std::string resolved_name() const {
            return string + ", id: " + std::to_string(id);
        }

        Node_kind get_node_kind() const {return kind;}

        virtual void print(std::ostream& stream) const {
            if(kind == TERMINAL){
                stream << string;

            } else {

                for(const std::shared_ptr<Node>& child : children){
                    stream << indentation_str << *child;
                } 
            }
        }

        friend std::ostream& operator<<(std::ostream& stream, const Node& n) {
            n.print(stream);
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

    protected:
        std::string string;
        U64 hash;
        Node_kind kind;
        int id;

        std::string indentation_str;
        std::vector<std::shared_ptr<Node>> children;
        Node_build_state state = NB_BUILD;
        std::optional<Node_constraint> constraint;
};

#endif
