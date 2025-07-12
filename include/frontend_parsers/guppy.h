#ifndef GUPPY_H
#define GUPPY_H

#include "../ast_builder/ast.h"

class Guppy : public Ast {

    public:
        using Ast::Ast;

        std::ofstream& write(std::ofstream& stream, const Node& node) override {
            std::string str = node.get_string();
            std::vector<std::shared_ptr<Node>> children = node.get_children();
            
            size_t num_children = children.size();

            if(node.get_node_kind() == TERMINAL){
                stream << str;
                // Always write tabs after newline so that the syntax is indented correctly
                if(str.back() == '\n'){
                    for(int i = 0; i < node.get_indent_depth(); i++){
                        stream << "\t";
                    }
                }            

            } else {

                U64 hash = node.get_hash();

                switch(hash){  
                    case Common::qubit_defs:
                        
                        for(size_t i = 0; i < num_children-1; i++){
                            write(stream, *children[i]);
                            stream << ", ";
                        }
                        
                        write(stream, *children[num_children-1]);

                        break;

                    case Common::imports:
                        write_children(stream, children, "\n");
                        break;

                    case Common::body:
                        //Indent first before first child
                        for(int i = 0; i < node.get_indent_depth(); i++){
                            stream << "\t";
                        }
                        write_children(stream, children);
                        break;
                        
                    default:
                        write_children(stream, children);
                        break;
                }

            }
        
            return stream;
        };

    private:

        
};



#endif
