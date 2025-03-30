#include "../include/ast.h"

std::string Common::terminal_value(const std::string& str){
    auto f = COMMON_TOKEN_STR.find((Common_token)hash_rule_name(str)); 

    if(f == COMMON_TOKEN_STR.end()){
        return str;
    } else {
        return f->second;
    }
}

/// @brief Given a rule, pick one branch from that rule
/// @param rule 
/// @return 
Result<Branch, std::string> Ast::pick_branch(std::shared_ptr<Rule> rule){
    float choice = random_float();
    float cummulative = 0.0;
    Result<Branch, std::string> result;

    std::vector<Branch> branches = rule->get_branches();

    if(branches.empty()){
        result.set_ok(Branch());
        return result;
    }
    
    // if we have done a set number of recursions already and this rule has a non recursive branch, choose that instead
    if ((recursions <= 0) && rule->get_recursive_flag()){
        result.set_ok(rule->pick_non_recursive_branch());   
        return result;
    }

    for(const Branch& b : branches){
        cummulative += b.get_prob();

        if(choice <= cummulative){
            result.set_ok(b);
            recursions -= 1;
            return result;
        }
    }

    result.set_error("[ERROR] Cannot pick branch for rule " + rule->get_name() + " !" + "\nChoose prob " + std::to_string(choice));
    return result;
}

void Ast::write_branch(std::shared_ptr<Node> node, const Result<Branch, std::string>& maybe_branch, int depth){

    if(maybe_branch.is_ok()){
        Branch branch = maybe_branch.get_ok();

        for(const Term& t : branch.get_terms()){
            std::shared_ptr<Node> child = std::make_shared<Node>(t,depth);
            
            if(t.is_pointer()){
                write_branch(child, pick_branch(t.get_rule()), depth + 1);
            }

            node->add_child(child);
        }

    } else {
        std::cout << maybe_branch.get_error() << std::endl;
    }
}

Result<Node, std::string> Ast::build(){

    std::shared_ptr<Node> root_ptr = std::make_shared<Node>(entry);
    Result<Node, std::string> res;

    if(entry == nullptr){
        res.set_error("Entry point not set");
        return res;

    } else {
        write_branch(root_ptr, pick_branch(entry), 1); // pick branch randomly to be written from entry point
        res.set_ok(*root_ptr);
        return res;
    }
}

int hash_rule_name(const std::string rule_name){
    int hash = 0;
    int len = 1;

    for(const char& c : rule_name){
        hash ^= c ^ (len++);
    }

    return hash;
}