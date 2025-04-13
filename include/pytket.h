#ifndef PYTKET_H
#define PYTKET_H

#include "ast.h"

namespace Pytket {

    struct Qreg {
        static int count;

        public:
            Qreg(size_t s){
                name = "qreg" + std::to_string(count++);
                size = s;
            }

            friend std::ostream& operator<<(std::ostream& stream, Qreg q){
                if(q.size == 1){ 
                    stream << q.name << "[0]";

                } else {
                    stream << q.name << "[" << q.size - 1 << ":0]";

                }

                return stream;
            }

        private:
            std::string name;
            size_t size;
    };

    class Pytket : public Ast {

        public:
            Pytket(){
                setup_qregs();
            }

            void add_constraint(std::shared_ptr<Node> node, Constraints::Constraints& constraints) override;

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

            void setup_qregs(){
                qregs.clear();

                int num_qubits = random_int(Common::MAX_QUBITS, Common::MIN_QUBITS);

                std::cout << num_qubits << "==============" << std::endl;
                
                while(num_qubits > 0){
                    size_t qreg_size = random_int(Common::MIN_QUBITS, 1);
                    qregs.push_back(Qreg(qreg_size));

                    num_qubits -= qreg_size;
                }

                for(Qreg& q : qregs){
                    std::cout << q << std::endl;
                }
            }

            std::ofstream& write(std::ofstream& stream, const Node& node) override {                
                // write terminal nodes right away
                if(node.is_terminal()){             
                    stream << Common::terminal_value(node.get_string());
                    return stream;
                }

                std::vector<std::shared_ptr<Node>> children = node.get_children();
            
                switch(node.get_hash()){
            
                    case Common::circuit_name:
                        stream << Common::TOP_LEVEL_CIRCUIT_NAME; break;

                    case Common::circuit: case Common::statements: case Common::qreg_defs:
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
        
            std::vector<Qreg> qregs;

    };
}

#endif

