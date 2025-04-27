#include "../include/qiskit.h"

/// @brief Depending on the type of the node, add constraints on which branch can be picked 
/// @param node 
/// @param constraints 
void Qiskit::add_constraint(std::shared_ptr<Node> node, Constraints::Constraints& constraints){

    switch(node->get_hash()){
        case Common::gate_name: constraints.clear(); break;

        case Common::circuit:
            Common::setup_qregs(qregs);
            qreg_definition_pointer = 0;
            constraints.add_constraint(Constraints::Constraint(Common::qreg_defs, Constraints::NUM_RULES_EQUALS, qregs.size()));
            break;

        case Common::h: case Common::x: case Common::y: case Common::z:
            constraints.add_n_qubit_constrait(1);
            break;
        
        case Common::cx:
            constraints.add_n_qubit_constrait(2);
            break;

        case Common::ccx:
            constraints.add_n_qubit_constrait(3);
            break;

        case Common::u1: case Common::rx: case Common::ry: case Common::rz:
            constraints.add_n_qubit_constrait(1, true);
            break;

        case Common::u2:
            constraints.add_n_qubit_constrait(2, true);            
            break;

        case Common::u3:            
            constraints.add_n_qubit_constrait(3, true);            
            break;   
    }
}
