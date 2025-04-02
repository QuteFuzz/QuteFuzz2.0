#include "../include/pytket.h"

/// @brief Depending on the type of the node, add constraints on which branch can be picked 
/// @param node 
/// @param constraints 
void Pytket::Pytket::add_constraint(std::shared_ptr<Node> node, Constraints::Constraints& constraints){

    switch(node->get_hash()){
        case gate_name: constraints.clear(); break;

        case Common::h: case Common::x: case Common::y: case Common::z:
            constraints.add_constraint(Constraints::Constraint(gate_application, Constraints::BRANCH_SIZE_EQUALS, 1)); 
            constraints.add_constraint(Constraints::Constraint(qubit_list, Constraints::BRANCH_SIZE_EQUALS, 1)); 
            break;
        
        case Common::cx:
            constraints.add_constraint(Constraints::Constraint(gate_application, Constraints::BRANCH_SIZE_EQUALS, 1)); 
            constraints.add_constraint(Constraints::Constraint(qubit_list, Constraints::BRANCH_SIZE_EQUALS, 2)); 
            break;

        case Common::ccx:
            constraints.add_constraint(Constraints::Constraint(gate_application, Constraints::BRANCH_SIZE_EQUALS, 1)); 
            constraints.add_constraint(Constraints::Constraint(qubit_list, Constraints::BRANCH_SIZE_EQUALS, 3)); 
            break;

        case Common::u1: case Common::rx: case Common::ry: case Common::rz:
            constraints.add_constraint(Constraints::Constraint(gate_application, Constraints::BRANCH_EQUALS, {float_literal, qubit_list})); 
            constraints.add_constraint(Constraints::Constraint(qubit_list, Constraints::BRANCH_SIZE_EQUALS, 1)); 
            break;

        case Common::u2:
            constraints.add_constraint(Constraints::Constraint(gate_application, Constraints::BRANCH_EQUALS, {float_literal, qubit_list})); 
            constraints.add_constraint(Constraints::Constraint(qubit_list, Constraints::BRANCH_SIZE_EQUALS, 2)); 
            break;

        case Common::u3:
            constraints.add_constraint(Constraints::Constraint(gate_application, Constraints::BRANCH_EQUALS, {float_literal, qubit_list})); 
            constraints.add_constraint(Constraints::Constraint(qubit_list, Constraints::BRANCH_SIZE_EQUALS, 3)); 
            break;   

    }
}

void Pytket::Pytket::write(fs::path& path) {
    Result<Node, std::string> maybe_ast_root = build(); 

    if(maybe_ast_root.is_ok()){
        Node ast_root = maybe_ast_root.get_ok();

        std::ofstream stream(path.string());

        write_imports(stream) << std::endl;

        write(stream, ast_root);

        std::cout << ast_root << std::endl;
        std::cout << "Written to " << path.string() << std::endl;

    } else {
        std::cout << "[ERROR] " << maybe_ast_root.get_error() << std::endl;
    }
};