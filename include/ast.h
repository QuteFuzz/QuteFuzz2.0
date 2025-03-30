#ifndef AST_H
#define AST_H

#include "term.h"
#include "grammar.h"

#define MAX_RECURSIONS 5

/// @brief A node is a terminal term with pointers to other nodes
class Node {

    public:
        Node(){}

        Node(const Term& _term, int _depth) : term(_term), depth(_depth){}

        Node(const std::shared_ptr<Rule> rule) {
            term.set_pointer(rule);
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

        std::string get_value() const {
            if(term.is_pointer()){
                return term.get_rule()->get_name();
            } else {
                return term.get_syntax();
            }
        }

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

        Result<Branch, std::string> pick_branch(std::shared_ptr<Rule> rule);

        virtual void write_branch(std::shared_ptr<Node> node, const Result<Branch, std::string>& maybe_branch, int depth);

        virtual Node build();

        virtual void write(fs::path& path) {
            std::ofstream stream(path.string());
            write(stream, build());
        };

    protected:
        virtual std::ofstream& write(std::ofstream& stream, const Node& node) {
            std::cout << "This grammar has no AST builder defined for it. Default builder has been used to create this AST" << std::endl;
            std::cout << "AST: \n" << node << std::endl;

            return stream;
        };

        int recursions = MAX_RECURSIONS;
        std::shared_ptr<Rule> entry = nullptr;

        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<float> float_dist;
};

int hash_rule_name(const std::string rule_name);

#endif
