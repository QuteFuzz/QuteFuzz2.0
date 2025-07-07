#ifndef AST_H
#define AST_H

#include <optional>
#include <algorithm>
#include "../grammar_parser/term.h"
#include "../grammar_parser/grammar.h"
#include "node.h"
#include "constraints.h"
#include "../qig/graph.h"
#include "block.h"
#include "context.h"

class Ast{
    public:
        Ast() : gen(rd()), float_dist(0.0, 1.0) {}

        ~Ast() = default;

        int build_counter = 0;

        void set_entry(const std::shared_ptr<Rule> _entry){
            entry = _entry;
        }   

        Result<Branch, std::string> pick_branch(const std::shared_ptr<Rule> rule);

        void transition_from_ready(std::shared_ptr<Node> node);

        void transition_from_init(std::shared_ptr<Node> node);

        void prepare_node(std::shared_ptr<Node> node);

        void write_branch(std::shared_ptr<Node> node);


        Result<Node, std::string> build(){
            Result<Node, std::string> res;
        
            if(entry == nullptr){
                res.set_error("Entry point not set");
        
            } else {
                context.reset();

                root_ptr = std::make_shared<Node>(entry, 0);

                write_branch(root_ptr);

                res.set_ok(*root_ptr);
            }

            return res;
        }

        /// @brief Try to find particular node by searching from root pointer
        /// @param hash 
        /// @return 
        std::shared_ptr<const Node> find(Common::Rule_hash hash){            
            return root_ptr->find(hash);
        }

        virtual void ast_to_program(fs::path output_dir, const std::string& extension, int num_programs);

    protected:

        /// @brief Simplest writer simply prints all terminals, or loops through all children until it eaches a terminal
        /// @param stream 
        /// @param node 
        /// @return 
        virtual std::ofstream& write(std::ofstream& stream, const Node& node) {
            std::string str = node.get_string();
            std::vector<std::shared_ptr<Node>> children = node.get_children();

            if(node.is_syntax()){
                stream << str;
            } else {
                write_children(stream, children);
            }
        
            return stream;
        };

        /// @brief Loop through and call `write` on each child of the given node
        /// @param stream 
        /// @param node
        /// @return 
        std::ofstream& write_children(std::ofstream& stream, 
            const std::vector<std::shared_ptr<Node>>& children, 
            std::string suffix = "")
        {
            for(auto child : children){
                write(stream, *child);
                stream << suffix;
            }

            return stream;
        }

        int recursions = 5;
        
        std::shared_ptr<Rule> entry = nullptr;
        std::shared_ptr<Node> root_ptr = nullptr;

        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<float> float_dist;

        Constraints::Constraints constraints;
        
        Context::Context context;
};

#endif
