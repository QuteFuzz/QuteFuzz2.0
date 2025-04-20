#ifndef PYTKET_H
#define PYTKET_H

#include "ast.h"

namespace Pytket {

    class Pytket : public Ast {

        public:
            using Ast::Ast;

            void add_constraint(std::shared_ptr<Node> node, Constraints::Constraints& constraints) override;

            void ast_to_program(fs::path& path) override;

        private:
            std::ofstream& write_imports(std::ofstream& stream){
                stream << "from sympy import Symbol \n" <<
                    "from helpers.pytket_helpers import test_circuit_on_passes \n" <<
                    "from pathlib import Path \n" <<
                    "from pytket import Circuit, Qubit, Bit \n" <<
                    "from pytket.circuit import Op, OpType, MultiplexorBox"
                << std::endl;
            
                return stream;
            }

            std::ofstream& write(std::ofstream& stream, const Node& node) override {     
                std::string str = node.get_string();
                auto hash = (Common::Common_token)node.get_hash(); 
                std::vector<std::shared_ptr<Node>> children = node.get_children();
                size_t num_children = children.size();

                if(node.is_terminal()){             
                    if(Common::is_common(hash)){
                        stream << Common::terminal_value(hash);
                    } else {
                        stream << str;
                    }
            
                    return stream;    
                }
            
                switch(hash){
            
                    case Common::circuit_name:
                        stream << Common::TOP_LEVEL_CIRCUIT_NAME; break;

                    case Common::imports:
                        write_imports(stream) << std::endl; break;

                    case Common::compiler_call: break;

                    case Common::circuit: case Common::statements: case Common::qreg_defs:

                        for(auto child : children){
                            write(stream, *child) << "\n";
                        }

                        break;

                    case Common::circuit_def: write_children(stream, children) << std::endl; break;
                
                    case Common::qreg_def:
                        qreg_definition = qregs.at(qreg_definition_pointer++);

                        write_children(stream, children);                        
                        break;
                    
                    case Common::qreg_name:
                        stream << qreg_definition.get_name();
                        break;

                    case Common::qreg_size:
                        stream << qreg_definition.get_size();
                        break;
                    
                    case Common::float_literal: stream << random_float(0.5); break;
        
                    default:
                        write_children(stream, children);
                }
            
                return stream;
            }
        
            std::vector<Common::Qreg> qregs;
            Common::Qreg qreg_definition;
            size_t qreg_definition_pointer = 0;

    };
}

#endif

