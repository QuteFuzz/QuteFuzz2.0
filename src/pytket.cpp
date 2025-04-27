#include "../include/pytket.h"

/// @brief These are additional constraints on nodes which we cannot add directly in the grammar. It makes sense that the nodes in here are basically all the "placeholders" in the 
/// grammar
/// @param node 
/// @param constraints 
void Pytket::add_constraint(std::shared_ptr<Node> node, Constraints::Constraints& constraints){
    auto hash = (Common::Rule_hash)node->get_hash(); 
    std::string str = node->get_string();

    switch(hash){

        case Common::imports:
            node->add_child(std::make_shared<Node>(imports(), node->get_depth() + 1));
            break;

        case Common::compiler_call:
            node->add_child(std::make_shared<Node>(compiler_call(), node->get_depth() + 1));
            break;

        case Common::qreg_name: {
            node->add_child(std::make_shared<Node>(qreg_to_write.get_name(), node->get_depth() + 1));
            break;
        }

        case Common::qreg_size: {
            node->add_child(std::make_shared<Node>(qreg_to_write.get_size_as_string(), node->get_depth() + 1));
            break;   
        }

        case Common::lparen: case Common::rparen: case Common::comma: case Common::space: case Common::dot: 
        case Common::single_quote: case Common::double_pipe: case Common::double_quote: case Common::double_ampersand: case Common::equals:
            node->add_child(std::make_shared<Node>(Common::terminal_value(hash), node->get_depth() + 1));
            break;

        case Common::circuit_name:
            node->add_child(std::make_shared<Node>(Common::TOP_LEVEL_CIRCUIT_NAME, node->get_depth() + 1));
            break;

        case Common::gate_name: 
            constraints.clear(); 
            qreg_defs.reset_qubits();
            break;

        case Common::circuit:
            Common::setup_qregs(qreg_defs);
            constraints.add_constraint(Constraints::Constraint(Common::qreg_defs, Constraints::NUM_RULES_EQUALS, qreg_defs.num_qregs()));
            break;            

        case Common::qreg_def:
            qreg_to_write = qreg_defs.get_next_qreg();
            break;

        case Common::qubit:
            node->add_child(std::make_shared<Node>(str, node->get_depth() + 1)); //TODO
            break;

        case Common::float_literal:
            node->add_child(std::make_shared<Node>(std::to_string(random_float(0.5)), node->get_depth() + 1));
            break;
            
        case Common::h: case Common::x: case Common::y: case Common::z:
            node->add_child(std::make_shared<Node>(str, node->get_depth() + 1));
            constraints.add_n_qubit_constrait(1);
            break;
        
        case Common::cx:
            node->add_child(std::make_shared<Node>(str, node->get_depth() + 1));
            constraints.add_n_qubit_constrait(2);
            break;

        case Common::ccx:
            node->add_child(std::make_shared<Node>(str, node->get_depth() + 1));
            constraints.add_n_qubit_constrait(3);
            break;

        case Common::u1: case Common::rx: case Common::ry: case Common::rz:
            node->add_child(std::make_shared<Node>(str, node->get_depth() + 1));
            constraints.add_n_qubit_constrait(1, true);
            break;

        case Common::u2:
            node->add_child(std::make_shared<Node>(str, node->get_depth() + 1));
            constraints.add_n_qubit_constrait(2, true);            
            break;

        case Common::u3:    
            node->add_child(std::make_shared<Node>(str, node->get_depth() + 1));
            constraints.add_n_qubit_constrait(3, true);            
            break;   

        
    }
}
