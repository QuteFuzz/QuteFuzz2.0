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

        std::shared_ptr<Node> get_node_from_term(const Term& term);

        Result<Node, std::string> build();

        virtual void ast_to_program(fs::path output_dir, const std::string& extension, int num_programs);

        void render_ast(const Node& root, const fs::path& current_circuit_dir);

    protected:

        // /// @brief Simplest writer simply prints all terminals, or loops through all children until it eaches a terminal
        // /// @param stream 
        // /// @param node 
        // /// @return 
        // virtual std::ofstream& write(std::ofstream& stream, const Node& node) {
        //     std::string str = node.get_string();
        //     std::vector<std::shared_ptr<Node>> children = node.get_children();

        //     if(node.get_node_kind() == TERMINAL){
        //         stream << str;
        //         if (str == "\n") {
        //             for(int i = 0; i < node.get_indent_depth(); i++){
        //                 stream << "\t";
        //             }
        //         }            

        //     } else {
        //         //Indent first before first child
        //         if (str == "indented_body") {
        //             for(int i = 0; i < node.get_indent_depth(); i++){
        //                 stream << "\t";
        //             } 
        //         }   
        //         write_children(stream, children);
        //     }
        
        //     return stream;
        // };
        
        std::shared_ptr<Rule> entry = nullptr;
        std::shared_ptr<Node> dummy = std::make_shared<Node>("");

        std::random_device rd;
        std::mt19937 gen;
        
        Context::Context context;
};

#endif
