#ifndef PYTKET_H
#define PYTKET_H

#include "ast.h"

namespace Pytket {
    typedef enum {
        CIRCUIT = 115,
        CIRCUIT_NAME = 7,
        CIRCUIT_DEF = 75,
        INT_LITERAL = 71,
        GATE_NAME = 78,
        H = 73,
        QUBIT_LIST = 45,
        PARAMETER_LIST = 59,
        PARAMETER = 104,
        FLOAT_LITERAL = 69,
        CX = 24,
        CCX = 88,
        IDENTIFIER = 8,
        STATEMENT = 100,
    } Rule_names;

    class Pytket : public Ast {

        public:
            using Ast::Ast;

            void write(fs::path& path) override;

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
                std::vector<std::shared_ptr<Node>> children = node.get_children();
                size_t children_size = children.size();
            
                switch(hash_rule_name(node.get_value())){
            
                    case CIRCUIT_NAME:
                        stream << circuit_name; break;

                    case CIRCUIT:
                        for(auto child : children){
                            write(stream, *child) << "\n";
                        }

                        break;
        
                    default:
                        if(node.is_terminal()){
                            stream << node.get_value();
                            return stream;
                        }

                        write_children(stream, node.get_children());
                }
            
                return stream;
            }

            std::string circuit_name = "main_circ";

    };
}

#endif

