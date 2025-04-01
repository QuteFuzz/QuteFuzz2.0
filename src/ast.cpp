#include "../include/ast.h"

int hash_rule_name(const std::string rule_name){
    int hash = 0;
    int len = 1;

    for(const char& c : rule_name){
        hash ^= c ^ (len++);
    }

    return hash;
}

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
Result<Branch, std::string> Ast::pick_branch(const std::shared_ptr<Rule> rule, Constraints::Constraints& c){
    Result<Branch, std::string> result;

    std::vector<Branch> branches = rule->get_branches();

    if(branches.empty()){
        result.set_ok(Branch());
        return result;
    }
    
    // if we have done a set number of recursions already and this rule has a non recursive branch, choose that instead
    if ((recursions <= 0) && rule->get_recursive_flag()){
        c.add_constraint({.type = Constraints::NON_RECURSIVE});
        result.set_ok(rule->pick_branch(c));   
        return result;
    }

    result.set_ok(rule->pick_branch(c));
    recursions -= 1;

    return result;
}

void Ast::write_branch(std::shared_ptr<Node> node, int depth){
    Term t = node->get_term();

    if(t.is_pointer()){

        Constraints::Constraints cs;

        Result<Branch, std::string> maybe_branch = pick_branch(t.get_rule(), cs);

        if(maybe_branch.is_ok()){
            Branch branch = maybe_branch.get_ok();
    
            for(const Term& t : branch.get_terms()){
                std::shared_ptr<Node> child = std::make_shared<Node>(t, depth);
                
                write_branch(child, depth + 1);
    
                node->add_child(child);
            }
    
        } else {
            std::cout << maybe_branch.get_error() << std::endl;
        }

    }
    
}

void Ast::write(fs::path& path) {
    std::ofstream stream(path.string());

    Result<Node, std::string> maybe_ast_root = build();

    if(maybe_ast_root.is_ok()){
        Node ast_root = maybe_ast_root.get_ok();
        write(stream, ast_root);

    } else {
        std::cout << "[ERROR] " << maybe_ast_root.get_error() << std::endl; 
    }

};
