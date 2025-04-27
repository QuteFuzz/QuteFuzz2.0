#ifndef QISKIT_H
#define QISKIT_H

#include "ast.h"

class Qiskit : public Ast {

    public:
        using Ast::Ast;

        void add_constraint(std::shared_ptr<Node> node, Constraints::Constraints& constraints) override;

    private:
        std::ofstream& write_imports(std::ofstream& stream){
            stream << "from qiskit import QuantumCircuit, QuantumRegister, ClassicalRegister \n" <<
                    "from qiskit.circuit import Parameter, ParameterVector \n" <<
                    "from helpers.qiskit_helpers import compare_statevectors, run_on_simulator, run_routing_simulation, run_pass_on_simulator \n" <<
                    "from pathlib import Path \n" <<
                    "from math import pi"
            << std::endl;
        
            return stream;
        }

        std::ofstream& write(std::ofstream& stream, const Node& node) override {     
            std::string str = node.get_string();
            auto hash = (Common::Rule_hash)node.get_hash(); 

            if(node.is_terminal()){             
                stream << str;            
                return stream;    
            }
        
            switch(hash){

                // PLACEHOLDERS
                case Common::lparen: case Common::rparen: case Common::comma: case Common::space: case Common::dot: 
                case Common::single_quote: case Common::double_pipe: case Common::double_quote: case Common::double_ampersand: case Common::equals:
                    stream << Common::terminal_value(hash);
                    break;
        
                case Common::circuit_name:
                    stream << Common::TOP_LEVEL_CIRCUIT_NAME; break;

                case Common::imports:
                    write_imports(stream) << std::endl; break;

                case Common::compiler_call: 
                    stream << "# [" << str << "] Not implemented" << std::endl;
                    break;
                
                case Common::qreg_name:
                    stream << qreg_definition.get_name();
                    break;

                case Common::qreg_size:
                    stream << qreg_definition.get_size();
                    break;
                
                case Common::float_literal: stream << random_float(0.5); break;
                
                case Common::h: case Common::x: case Common::y: case Common::z: case Common::rz: case Common::rx: case Common::ry: 
                    case Common::cx: case Common::ccx: case Common::u:
                    stream << str; 
                    break;

                case Common::qubit: stream << str; break; // TODO

                // RULE POINTERS (only interested in the children)

                case Common::program: case Common::int_literal: case Common::gate_name: case Common::qubit_list: case Common::parameter_list: 
                case Common::parameter: case Common::gate_application: case Common::gate_application_kind: case Common::statement: case Common::circuit_def: case Common::qreg_defs:
                    write_children(stream, node);
                    break;

                case Common::circuit: case Common::statements:
                    write_children(stream, node, "\n");
                    break;
            
                case Common::qreg_def:
                    qreg_definition = qregs.at(qreg_definition_pointer++);
                    write_children(stream, node, "\n");                        
                    break;
                
                case Common::qreg_decl:
                    write_children(stream, node);
                    break;

                case Common::qreg_append:
                    write_children(stream, node);
                    break;

                default:
                    std::cerr << "Unknown rule " << str << std::endl;
            }
        
            return stream;
        }
    
        std::vector<Common::Qreg> qregs;
        Common::Qreg qreg_definition;
        size_t qreg_definition_pointer = 0;

};

#endif