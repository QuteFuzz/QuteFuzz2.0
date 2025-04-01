#include "../include/pytket.h"

/// @brief Depending on the type of the node, add constraints on which branch can be picked 
/// @param node 
/// @param constraints 
void Pytket::Pytket::add_constraint(std::shared_ptr<Node> node, Constraints::Constraints& constraints){
    // std::cout << "Node " << node->get_string() << " has value " << node->get_value() << std::endl;
    switch(node->get_value()){
        case gate_name: constraints.clear(); break;
        case Common::h: 
            constraints.add_constraint({.type = Constraints::EQUALS, .value = 1, .node = qubit_list}); break;
        case Common::cx:
            constraints.add_constraint({.type = Constraints::EQUALS, .value = 2, .node = qubit_list}); break;

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