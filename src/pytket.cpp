#include "../include/pytket.h"


void Pytket::Pytket::write_branch(std::shared_ptr<Node> node, int depth, Constraints::Constraints& constraints){
    Term t = node->get_term();

    if(t.is_pointer()){
        // figure out what constraints to add to branch depending on the node type
        switch(node->get_value()){
            case gate_name: constraints.clear(); break;
            case h: 
                constraints.add_constraint({.type = Constraints::EQUALS, .value = 1, .node = qubit_list}); std::cout << "constraint on qubit_list " << std::endl; break;
            case cx:
                constraints.add_constraint({.type = Constraints::EQUALS, .value = 2, .node = qubit_list}); std::cout << "constraint on qubit_list " << std::endl; break;

        }

        Result<Branch, std::string> maybe_branch = pick_branch(t.get_rule(), constraints);

        if(maybe_branch.is_ok()){
            Branch branch = maybe_branch.get_ok();
    
            for(const Term& t : branch.get_terms()){
                std::shared_ptr<Node> child = std::make_shared<Node>(t, depth);
                
                write_branch(child, depth + 1, constraints);
    
                node->add_child(child);
            }
    
        } else {
            std::cout << maybe_branch.get_error() << std::endl;
        }

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