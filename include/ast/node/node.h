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


        Node(const std::string _string, const U64 _hash, const Node_constraint& _constraint, const std::string _indentation_str = ""):
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
        std::string get_debug_constraint_string() const {
            if(constraint.has_value()){
                std::string debug_string;

                for(size_t i = 0; i < constraint.value().rules_size(); i++){
                    unsigned int n_occurances = constraint.value().get_occurances(i);
                    
                    debug_string += std::to_string(constraint.value().get_rule(i)) + " with occurances: " + std::to_string(n_occurances) + " ";
                    
                    if(n_occurances > (unsigned int)WILDCARD_MAX){
                        debug_string += RED("(Cannot be satisfied! Max = " + std::to_string(WILDCARD_MAX) + ")");
                    }
                }

                return debug_string;
            
            } else {
                return "no constraint";
            
            }
        }
        #endif

        virtual unsigned int get_n_ports() const {return 1;}

        std::shared_ptr<Node> find(const U64 _hash) const;

        inline bool is_subroutine_gate() const {return hash == Common::subroutine;}

        int get_next_qubit_op_target(){
            size_t partition_size = qubit_op_target_partition.size();

            if(partition_counter < partition_size){
                return qubit_op_target_partition[partition_counter++];
            } else {
                WARNING("Node " + string + " qubit node target partition info: Counter: " + std::to_string(partition_counter) + ", Size: " + std::to_string(partition_size));
                return 1;
            }
        }

        /// @brief Create a random partition of `target` over `n_children`. Final result contains +ve ints
        /// @param target 
        /// @param n_children 
        void make_partition(int target, int n_children){

            if((n_children == 1) || (target == 1)){
                qubit_op_target_partition = {target};
            
            } else if (target == n_children){
                qubit_op_target_partition = std::vector<int>(n_children, 1);

            } else {

                /*
                    make N-1 random cuts between 1 and T-1
                    ex: 
                        T = 10, N = 4
                        {2, 9, 4}
                */
                std::vector<int> cuts;

                for(int i = 0; i < n_children-1; i++){
                    int val = random_int(target-1, 1);

                    while(std::find(cuts.begin(), cuts.end(), val) != cuts.end()){
                        val = random_int(target-1, 1);
                    }

                    cuts.push_back(val);
                }

                /*
                    sort the cuts
                    ex:
                        {2, 4, 9}
                */
                std::sort(cuts.begin(), cuts.end());

                /*
                    add 0 and T boundaries, then calculate diffs
                    ex:
                        {0, 2, 4, 9, 10}
                        {2, 2, 5, 1} <- result
                */
                qubit_op_target_partition.push_back(cuts[0]);

                for(int i = 1; i < n_children-1; i++){
                    qubit_op_target_partition.push_back(cuts[i] - cuts[i-1]);
                }

                qubit_op_target_partition.push_back(target - cuts[n_children-2]);

            }
        
        #ifdef DEBUG
            std::cout << "Partition at " << string << std::endl;
            for(size_t i = 0; i < qubit_op_target_partition.size(); i++){
                std::cout << qubit_op_target_partition[i] << " ";
            }

            std::cout << std::endl;
        #endif
        }

        void make_control_flow_partition(int target, int n_children){
            make_partition(target, n_children);
            
            if(n_children == 1){
                add_constraint(Common::else_stmt, 0);
                add_constraint(Common::elif_stmt, 0);
            } else if (random_int(1)) {
                add_constraint(Common::else_stmt, 1);
            } else {
                add_constraint(Common::elif_stmt, 1);
            }
        }

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
