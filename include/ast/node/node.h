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
        Node_constraint(Common::Rule_hash _rule, unsigned int _occurances):
            rules({_rule}),
            occurances({_occurances})
        {}

        Node_constraint(std::vector<Common::Rule_hash> _rules, std::vector<unsigned int> _occurances): 
            rules(std::move(_rules)),
            occurances(std::move(_occurances))
        {}

        bool passed(const Branch& branch){
            // Count the number of occurances of each rule in the branch and return true if they match the expected occurances
            for(size_t i = 0; i < rules.size(); i++){
                if(branch.count_rule_occurances(rules[i]) != occurances[i]){
                    return false;
                }
            }
            return true;
        }

        Common::Rule_hash get_rule(unsigned int index) const {
            return rules[index];
        }

        size_t get_occurances(unsigned int index) const {
            return occurances[index];
        }

        size_t rules_size() const {
            return rules.size();
        }

        bool contains_rule(const Common::Rule_hash& rule) const {
            return std::find(rules.begin(), rules.end(), rule) != rules.end();
        }

        void add(const Common::Rule_hash& rule, unsigned int n_occurances){
            rules.push_back(rule);
            occurances.push_back(n_occurances);    
        }

    private:
        std::vector<Common::Rule_hash> rules;
        std::vector<unsigned int> occurances = {0};

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


        Node(const std::string _string, const U64 _hash, const std::optional<Node_constraint>& _constraint, const std::string _indentation_str = ""):
            string(_string),
            hash(_hash),
            kind((hash == 0ULL) ? TERMINAL : NON_TERMINAL),
            indentation_str(_indentation_str),
            constraint(_constraint)
        {
            id = node_counter++;
        }

        virtual ~Node() = default;

        inline void add_child(const std::shared_ptr<Node> child){
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

        U64 get_hash() const {
            return hash;
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

        inline std::shared_ptr<Node> child_at(size_t index) const {
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

        void set_constraint(std::vector<Common::Rule_hash> rules, std::vector<unsigned int> occurances){
            if(rules.size() != occurances.size()){
                ERROR("Hashes vector must be the same size as occurances vector!");
            }

            constraint = std::make_optional<Node_constraint>(rules, occurances);
        }

        void add_constraint(const Common::Rule_hash& rule, unsigned int n_occurances){
            if(constraint.has_value()){
                constraint.value().add(rule, n_occurances);
            } else {
                constraint = std::make_optional<Node_constraint>(rule, n_occurances);
            }
        }

        #ifdef DEBUG
        std::string get_debug_constraint_string() const;
        #endif

        virtual unsigned int get_n_ports() const {return 1;}

        std::shared_ptr<Node> find(const U64 _hash) const;

        inline bool is_subroutine_gate() const {return hash == Common::subroutine;}

        int get_next_qubit_op_target();

        void make_partition(int target, int n_children);

        void make_control_flow_partition(int target, int n_children);

        inline void set_from_dag(){from_dag = true;}

        inline bool is_from_dag(){return from_dag;}

    protected:
        std::string string;
        U64 hash;
        Node_kind kind;
        int id;

        std::string indentation_str;
        std::vector<std::shared_ptr<Node>> children;
        Node_build_state state = NB_BUILD;
        std::vector<int> qubit_op_target_partition;
        unsigned int partition_counter = 0;

        bool from_dag = false;
    
    private:
        std::optional<Node_constraint> constraint;
};

#endif
