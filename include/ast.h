#ifndef AST_H
#define AST_H

#include "term.h"
#include "grammar.h"
#include "constraints.h"

#define MAX_RECURSIONS 5

/// @brief A node is a term with pointers to other nodes
class Node {

    public:
        Node(){}

        /// @brief Create node from a term. 
        /// @param _term 
        /// @param _depth 
        Node(const Term& _term) : term(_term){}

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
            num_children++;
        }

        std::string indent() const {
            std::string tabs = "";

            for(int i = 0; i < depth; ++i){
                tabs += "\t";
            }

            return tabs;
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

        /// @brief A node is terminal if it is a syntax node, or if it is a pointer pointing to nothing
        /// @return 
        bool is_terminal() const {
            return term.is_syntax();
        }

        friend std::ostream& operator<<(std::ostream& stream, const Node& n) {
            stream << "[" << n.term << "]" << " children: " << n.num_children
                << " depth: " << n.depth
                << std::endl;

            for(auto child : n.children){
                stream << n.indent() << "->" << *child;
            }

            return stream;
        }

        std::vector<std::shared_ptr<Node>> get_children() const {
            return children;
        }

        int get_depth(){
            return depth;
        }

        void set_depth(int _depth){
            depth = _depth;
        }

    private:
        Term term;
        int depth = 0;
        std::vector<std::shared_ptr<Node>> children;
        size_t num_children = 0;

};

class Ast{
    public:
        Ast() : gen(rd()), float_dist(0.0, 1.0) {}

        ~Ast() = default;

        void set_entry(const std::shared_ptr<Rule> _entry){
            entry = _entry;
        }   

        Result<Branch, std::string> pick_branch(const std::shared_ptr<Rule> rule, Constraints::Constraints& constraints);

        void add_constraint(std::shared_ptr<Node> node, Constraints::Constraints& constraints);

        void write_branch(std::shared_ptr<Node> node, Constraints::Constraints& constraints);

        Result<Node, std::string> build(){
            Result<Node, std::string> res;
        
            if(entry == nullptr){
                res.set_error("Entry point not set");
                return res;
        
            } else {
                std::shared_ptr<Node> root_ptr = std::make_shared<Node>(entry, 0);

                Constraints::Constraints constraints;

                write_branch(root_ptr, constraints);
                res.set_ok(*root_ptr);
                return res;
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

            if(node.is_terminal()){
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

        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<float> float_dist;

        Common::Qreg_definitions qreg_defs;
        Common::Qreg qreg_to_write = Common::Qreg();
};

#endif
