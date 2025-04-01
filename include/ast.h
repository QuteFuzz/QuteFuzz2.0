#ifndef AST_H
#define AST_H

#include "term.h"
#include "grammar.h"

#define MAX_RECURSIONS 5

/// @brief A node is a term with pointers to other nodes
class Node {

    public:
        Node(){}

        Node(const Term& _term, int _depth) : term(_term), depth(_depth){}

        Node(const std::shared_ptr<Rule> rule) {
            term.set(rule);
        }

        void add_child(const std::shared_ptr<Node> child){
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

        uint64_t get_value() const {
            return term.get_value();
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
            return term.is_syntax() || !num_children;
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

    private:
        Term term;
        int depth = 0;
        std::vector<std::shared_ptr<Node>> children;
        size_t num_children = 0;

};

class Ast{
    public:
        Ast() : gen(rd()), float_dist(0.0, 1.0) {}

        inline float random_float(){
            return float_dist(gen);
        }

        void set_entry(const std::shared_ptr<Rule> _entry){
            entry = _entry;
        }

        /// @brief Pick a branch that satisfies some constraint
        /// @param rule 
        /// @param c 
        /// @return 
        Result<Branch, std::string> pick_branch(const std::shared_ptr<Rule> rule, Constraints::Constraints& constraints);

        virtual void add_constraint(std::shared_ptr<Node> node, Constraints::Constraints& constraints){};

        void write_branch(std::shared_ptr<Node> node, int depth, Constraints::Constraints& constraints);

        Result<Node, std::string> build(){

            std::shared_ptr<Node> root_ptr = std::make_shared<Node>(entry);
            Result<Node, std::string> res;
        
            if(entry == nullptr){
                res.set_error("Entry point not set");
                return res;
        
            } else {
                Constraints::Constraints constraints;

                write_branch(root_ptr, 1, constraints);
                res.set_ok(*root_ptr);
                return res;
            }
        }

        virtual void write(fs::path& path);

    protected:
        virtual std::ofstream& write(std::ofstream& stream, const Node& node) {
            std::cout << "This grammar has no AST builder defined for it. Default builder has been used to create this AST" << std::endl;
            std::cout << "AST: \n" << node << std::endl;
        
            return stream;
        };

        /// @brief Loop through and call `write` on each child 
        /// @param stream 
        /// @param children 
        /// @return 
        std::ofstream& write_children(std::ofstream& stream, const std::vector<std::shared_ptr<Node>> children){
            for(auto child : children){
                write(stream, *child);
            }

            return stream;
        }

        int recursions = MAX_RECURSIONS;
        std::shared_ptr<Rule> entry = nullptr;

        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<float> float_dist;
};

#endif
