#ifndef PYTKET_H
#define PYTKET_H

#include "ast.h"

namespace Pytket {
    const std::string TOP_LEVEL_CIRCUIT = "main_circ"; 

    enum Rule_names : uint64_t {
        circuit = 18088473315674432532ULL,
        circuit_name = 5389401364268778602ULL,
        circuit_def = 17654104105659481736ULL,
        int_literal = 12222978820271297122ULL,
        gate_name = 4107851538286704628ULL,
        qubit_list = 18380990572907722739ULL,
        parameter_list = 10044088521670889753ULL,
        parameter = 1363275014107747824ULL,
    } ;

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
                
                // write terminal nodes right away
                if(node.is_terminal()){             
                    stream << Common::terminal_value(node.get_string());
                    return stream;
                }

                std::vector<std::shared_ptr<Node>> children = node.get_children();
            
                switch(node.get_value()){
            
                    case circuit_name:
                        stream << TOP_LEVEL_CIRCUIT; break;

                    case circuit:    
                        std::cout << "circuit rule " << node.get_string() << std::endl;                     
                        for(auto child : children){
                            write(stream, *child) << "\n";
                        }

                        break;
        
                    default:
                        write_children(stream, node.get_children());
                }
            
                return stream;
            }

    };
}

#endif

