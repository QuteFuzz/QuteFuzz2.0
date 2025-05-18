#ifndef AST_H
#define AST_H

#include <memory>
#include <stack>

#include "term.h"
#include "grammar.h"
#include "constraints.h"

#define MAX_RECURSIONS 5
#define INDENT_STR "---"

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

};

enum ND_Node_kind {
    ND_INIT,
    ND_COMP,
};


struct Node_dependency {
    public:
        Node_dependency(U64 init, U64 comp){
            initiator_hash = init;
            completer_hash = comp;
        }

        bool node_is(ND_Node_kind kind, U64 node_hash) const {
            switch(kind){
                case ND_INIT: return initiator_hash == node_hash;
                case ND_COMP: return completer_hash == node_hash;
                default: return false;
            }
        }

        U64 get(ND_Node_kind kind) const {
            switch(kind){
                case ND_INIT: return initiator_hash;
                case ND_COMP: return completer_hash;
                default: return 0;
            }
        }

        bool is_initiator_done(){return initiator_done;}

        void set_initiator_done(){initiator_done = true;}

        void increment_info(){
            info++;
            info_set = true;
        }

        int get_info(){return info;}

        bool is_info_set(){return info_set;}

        void reset(){
            initiator_done = false;
            info_set = false;
            info = 0;
        }

    private:
        U64 initiator_hash = 0ULL;
        U64 completer_hash = 0ULL;
        bool initiator_done = false;
        bool info_set = false;
        int info = 0;
};


/// @brief Relationship between initiators and completers is many to one
class Ast{
    public:
        Ast() : gen(rd()), float_dist(0.0, 1.0) {}

        ~Ast() = default;

        void set_entry(const std::shared_ptr<Rule> _entry){
            entry = _entry;
        }   

        Result<Branch, std::string> pick_branch(const std::shared_ptr<Rule> rule, Constraints::Constraints& constraints);

        /// @brief Ensures that many-to-one relationship between initiators and completers is maintained. (Cannot have repeating completers)
        /// @param init 
        /// @param comp 
        void add_node_dependency(U64 init, U64 comp){
            for(const Node_dependency& nd : node_deps){
                // cannot have the same initiator more than once
                if(nd.node_is(ND_INIT, comp)){return;}
            }

            node_deps.push_back(Node_dependency(init, comp));
        }

        void resolve_dependency(std::shared_ptr<Node> initiator_node, Node_dependency& nd, Constraints::Constraints& constraints);

        Node_build_state transition_from_ready(std::shared_ptr<Node> node, Branch& chosen_branch);

        Node_build_state transition_from_init(std::shared_ptr<Node> node, Constraints::Constraints& constraints);

        void add_constraint(std::shared_ptr<Node> node, Constraints::Constraints& constraints);

        void write_branch(std::shared_ptr<Node> node, Constraints::Constraints& constraints);

        Result<Node, std::string> build(){
            Result<Node, std::string> res;

            reset_dependency_flags();
        
            if(entry == nullptr){
                res.set_error("Entry point not set");
                return res;
        
            } else {
                root_ptr = std::make_shared<Node>(entry, 0);

                Constraints::Constraints constraints;
                write_branch(root_ptr, constraints);

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

        /// @brief Reset flags on new AST build that control dependency resolution
        void reset_dependency_flags(){
            for(Node_dependency& nd : node_deps){
                nd.reset();
            }

            
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

        int recursions = MAX_RECURSIONS;
        std::shared_ptr<Rule> entry = nullptr;

        std::shared_ptr<Node> root_ptr = nullptr;

        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<float> float_dist;

        Common::Qreg_definitions qreg_defs;
        std::shared_ptr<Common::Qreg> qreg_to_write = Common::DEFAULT_QREG;
        std::shared_ptr<Common::Qubit> qubit_to_write = Common::DEFAULT_QUBIT;

        std::vector<Node_dependency> node_deps = {
            Node_dependency(Common::qreg_defs, Common::gate_application),
        };
        std::stack<std::shared_ptr<Node_dependency>> dependencies_in_flight;
        

};

#endif
