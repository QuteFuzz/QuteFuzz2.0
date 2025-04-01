#ifndef PYTKET_H
#define PYTKET_H

#include "ast.h"

namespace Pytket {
    const std::string TOP_LEVEL_CIRCUIT = "main_circ"; 

    typedef enum {
        circuit = 115,
        circuit_name = 39,
        circuit_def = 75,
        int_literal = 71,
        gate_name = 78,
        h = 73,
        qubit_list = 45,
        parameter_list = 59,
        parameter = 104,
        float_literal = 69,
        cx = 24,
        ccx = 120,
        identifier = 8,
        statement = 100,
    } Rule_names;

    class Pytket : public Ast {

        public:
            using Ast::Ast;

            void write_branch(std::shared_ptr<Node> node, int depth, Constraints::Constraints& constraints) override;

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
            
                switch(node.get_value()){
            
                    case circuit_name:
                        stream << TOP_LEVEL_CIRCUIT; break;

                    case circuit:
                        stream << "# CIRCUIT" << std::endl;
                         
                        for(auto child : children){
                            write(stream, *child) << "\n";
                        }

                        break;
        
                    default:
                        if(node.is_terminal()){                        
                            stream << Common::terminal_value(node.get_string());
                            return stream;
                        }

                        write_children(stream, node.get_children());
                }
            
                return stream;
            }

    };
}

#endif

