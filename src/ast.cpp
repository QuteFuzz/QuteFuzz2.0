#include "../include/ast.h"

/// @brief Given a rule, pick one branch from that rule
/// @param rule 
/// @return 
Result<Branch, std::string> Ast::pick_branch(const std::shared_ptr<Rule> rule, Constraints::Constraints& constraints){
    Result<Branch, std::string> result;

    std::vector<Branch> branches = rule->get_branches();

    if(branches.empty()){
        result.set_ok(Branch());
        return result;
    }
    
    // if we have done a set number of recursions already and this rule has a non recursive branch, choose that instead
    if ((recursions <= 0) && rule->get_recursive_flag()){
        constraints.add_constraint(Constraints::Constraint(Constraints::BRANCH_IS_NON_RECURSIVE));
        result.set_ok(rule->pick_branch(constraints));   
        return result;
    }

    result.set_ok(rule->pick_branch(constraints));
    recursions -= 1;

    return result;
}

/// @brief Write branch terms to node children. Choosen branch must satisfy given constraints
/// @param node 
/// @param depth 
/// @param constraints 
void Ast::write_branch(std::shared_ptr<Node> node, Constraints::Constraints& constraints){
    Term t = node->get_term();

    if(t.is_pointer()){
        add_constraint(node, constraints);

        Result<Branch, std::string> maybe_branch = pick_branch(t.get_rule(), constraints);

        if(maybe_branch.is_ok()){
            Branch branch = maybe_branch.get_ok();
    
            for(const Term& t : branch.get_terms()){
                std::shared_ptr<Node> child = std::make_shared<Node>(t, node->get_depth() + 1);
                
                write_branch(child, constraints);
    
                node->add_child(child);
            }
    
        } else {
            std::cout << maybe_branch.get_error() << std::endl;
        }

    }
    
}

void Ast::ast_to_program(fs::path& path) {

    Result<Node, std::string> maybe_ast_root = build();

    if(maybe_ast_root.is_ok()){
        Node ast_root = maybe_ast_root.get_ok();
        std::ofstream stream(path.string());

        write(stream, ast_root);

        std::cout << ast_root << std::endl;
        std::cout << "Written to " << path.string() << std::endl;

    } else {
        std::cout << "[ERROR] " << maybe_ast_root.get_error() << std::endl; 
    }

};
