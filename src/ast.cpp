#include "../include/ast.h"

/// @brief Given a rule, pick one branch from that rule
/// @param rule 
/// @return 
Result<Branch, std::string> Ast_builder::pick_branch(std::shared_ptr<Rule> rule){
    float choice = random_float();
    float cummulative = 0.0;
    Result<Branch, std::string> result;

    std::vector<Branch> branches = rule->get_branches();

    if(branches.empty()){
        result.set_ok(Branch());
        return result;
    }
    
    // if we have done a set number of recursions already and this rule has a non recursive branch, choose that instead
    if ((depth <= 0) && rule->get_recursive_flag()){
        result.set_ok(rule->pick_non_recursive_branch());   
        return result;
    }

    for(const Branch& b : branches){
        cummulative += b.get_prob();

        if(choice <= cummulative){
            result.set_ok(b);
            depth -= 1;
            return result;
        }
    }

    result.set_error("[ERROR] Cannot pick branch for rule " + rule->get_name() + " !" + "\nChoose prob " + std::to_string(choice));
    return result;
}

void Ast_builder::write_branch(std::shared_ptr<Node> node, const Result<Branch, std::string>& maybe_branch){

    if(maybe_branch.is_ok()){
        Branch branch = maybe_branch.get_ok();

        for(const Term& t : branch.get_terms()){
            std::shared_ptr<Node> child = std::make_shared<Node>(t, 0);
            
            if(t.is_pointer()){
                write_branch(child, pick_branch(t.get_rule()));
            }

            node->add_child(child);
        }

    } else {
        std::cout << maybe_branch.get_error() << std::endl;
    }
}

Node Ast_builder::emit(){
    std::shared_ptr<Rule> entry = grammar.get_rule_pointer(entry_point);
    std::shared_ptr<Node> root = std::make_shared<Node>(entry);

    write_branch(root, pick_branch(entry)); // pick branch randomly to be written from entry point

    std::cout << *root << std::endl;

    return *root;
}
