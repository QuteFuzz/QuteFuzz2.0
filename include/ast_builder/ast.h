#ifndef AST_H
#define AST_H

#include <optional>
#include <algorithm>
#include <term.h>
#include <grammar.h>
#include <node.h>
#include <graph.h>
#include <context.h>

class Ast{
    public:
        Ast() : gen(rd()) {}

        ~Ast() = default;

        int build_counter = 0;

        void set_entry(const std::shared_ptr<Rule> _entry){
            entry = _entry;
        }   

        void write_branch(std::shared_ptr<Node> parent, const Term& term);

        std::shared_ptr<Node> get_node_from_term(const std::shared_ptr<Node> parent, const Term& term);

        Result<Node> build();

        virtual void ast_to_program(fs::path output_dir, const std::string& extension, int num_programs);

        void render_ast(const Node& root, const fs::path& current_circuit_dir);

    protected:

        std::shared_ptr<Rule> entry = nullptr;
        std::shared_ptr<Node> dummy = std::make_shared<Node>("");

        std::random_device rd;
        std::mt19937 gen;
        
        Context::Context context;
};

#endif
