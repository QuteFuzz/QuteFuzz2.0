#ifndef AST_H
#define AST_H

#include <memory>

#include "term.h"
#include "grammar.h"
#include "constraints.h"

#define MAX_RECURSIONS 5
#define INDENT_STR "---"

enum Node_build_state {
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
                << " depth: " << n.depth;

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
        Node_build_state nb = NB_STALL;
        Branch chosen_branch;
        bool chosen_branch_flag = false;

};


struct Node_dependency {
    public:
        Node_dependency(U64 init, U64 comp){
            initiator_hash = init;
            completer_hash = comp;
        }

        bool node_is_completer(U64 node_hash) const {
            return completer_hash == node_hash;
        }

        bool node_is_initiator(U64 node_hash) const {
            return initiator_hash == node_hash;
        }

        U64 get_initiator_hash(){return initiator_hash;}

        U64 get_completer_hash(){return completer_hash;}

        bool is_initiator_done(){return initiator_done;}

        bool is_completer_children_set(){return completer_children_set;}

        void set_initiator_done(){initiator_done = true;}

        void set_num_completer_node_children(size_t n){
            num_completer_node_children = n;
            completer_children_set = true;
        }

        size_t get_num_completer_node_children(){return num_completer_node_children;}

        void reset(){
            initiator_done = false;
            completer_children_set = false;
        }

    private:
        U64 initiator_hash = 0ULL;
        U64 completer_hash = 0ULL;
        bool initiator_done = false;
        bool completer_children_set = false;
        size_t num_completer_node_children = 0;
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
                if(nd.node_is_completer(comp)){return;}
            }

            node_deps.push_back(Node_dependency(init, comp));
        }

        void resolve_dependency(std::shared_ptr<Node> initiator_node, Node_dependency& nd, Constraints::Constraints& constraints);

        size_t get_num_completer_node_children(U64 node_hash){
            for(Node_dependency& nd : node_deps){
                if(nd.node_is_initiator(node_hash)){
                    return nd.get_num_completer_node_children();
                }
            }

            return 0;
        }

        Node_build_state node_ready_to_done(std::shared_ptr<Node> node, Branch& chosen_branch);

        Node_build_state node_stall_to_ready(std::shared_ptr<Node> node, Constraints::Constraints& constraints);

        void add_constraint(std::shared_ptr<Node> node, Constraints::Constraints& constraints);

        void write_branch(std::shared_ptr<Node> node, Constraints::Constraints& constraints);

        Result<Node, std::string> build(){
            Result<Node, std::string> res;

            add_node_dependency(Common::qreg_defs, Common::statements);

            reset();
        
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

        void reset(){
            // constraints.safe_clear();

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

        std::vector<Node_dependency> node_deps;

};

#endif
