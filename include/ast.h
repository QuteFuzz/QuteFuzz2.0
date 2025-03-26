#ifndef AST_H
#define AST_H

#include "term.h"
#include "grammar.h"

#define MAX_DEPTH 5

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

        friend std::ostream& operator<<(std::ostream& stream, const Node& n) {
            stream << "[" << n.term << "]" << " children: " << n.num_children
                << " depth: " << n.depth
                << std::endl;

            for(auto child : n.children){
                stream << "->" << *child;
            }

            return stream;
        }

    private:
        Term term;
        int depth = 0;
        std::vector<std::shared_ptr<Node>> children;
        size_t num_children = 0;

};

class Ast_builder{
    public:

        Ast_builder(const fs::path& filename, const std::string& _entry_point) : grammar(filename), entry_point(_entry_point), gen(rd()), float_dist(0.0, 1.0) {
            grammar.build_grammar();
            grammar.print_grammar();
        }

        inline float random_float(){
            return float_dist(gen);
        }

        void set_grammar(const Grammar& _grammar, const std::string& _entry_point){
            grammar = _grammar;
            entry_point = _entry_point;
        }

        void write_branch(std::shared_ptr<Node> node, const Result<Branch, std::string>& maybe_branch);

        Result<Branch, std::string> pick_branch(std::shared_ptr<Rule> rule);

        Node emit();

    private:
        Grammar grammar;

        std::string entry_point;
        int depth = MAX_DEPTH;

        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<float> float_dist;
};

#endif
