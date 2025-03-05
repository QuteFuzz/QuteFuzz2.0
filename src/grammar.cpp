#include "../include/grammar.h"

Grammar::Grammar(const fs::path& filename): lexer(filename.string()), name(filename.stem()), path(filename) {
    lexer.lex();

    tokens = lexer.get_tokens();

    num_tokens = tokens.size();
}

void Grammar::consume(int n){

    token_pointer += n;
            
    if(token_pointer == num_tokens){
        curr_token.set_error("Out of tokens!");
    } else {

        curr_token.set_ok(tokens[token_pointer]);
    }
}

void Grammar::consume(const Token_kind kind){

    if(curr_token.get_ok().kind == kind){
        consume(1);
    } else {
        curr_token.set_error("Expected syntax not matched");
    }
}

void Grammar::peek(){
    if((token_pointer + 1) == num_tokens){
        next_token.set_error("Cannot peek!");
    } else {
        next_token.set_ok(tokens[token_pointer+1]);
    }

}

std::shared_ptr<Rule> Grammar::get_rule_pointer(const std::string& rule_name){

    if(rule_pointers.find(rule_name) != rule_pointers.end()){
        return rule_pointers[rule_name];
    } else {
        std::shared_ptr<Rule> ptr = std::make_shared<Rule>(rule_name);

        rule_pointers[rule_name] = ptr;

        return ptr;
    }
}

void Grammar::build_branch(Branch& branch, const Token& token){
    Term term(token.value);
    std::shared_ptr<Rule> ptr;
    
    if(token.kind == T_SYNTAX){
        term.set_syntax(token.value);
    } else {
        term.set_pointer(get_rule_pointer(token.value));
    }

    branch.add(term);
}

bool Grammar::is_rule_end(){
    peek();

    if(next_token.is_error()){
        return true;
    } else {
        return (next_token.get_ok().kind == T_RULE_START);
    }
}

bool Grammar::is_grammar_end(){

    return (curr_token.get_ok().kind == T_EOF);
}

void Grammar::build_rule(std::shared_ptr<Rule> current_rule){
    Branch b;
    Token token;

    while(curr_token.is_ok() && !is_rule_end()){
        token = curr_token.get_ok();

        if(token.kind == T_SEPARATOR){
            // need to create new branch
            b.set_recursive_flag(current_rule); // set whether or not this branch is non-recursive
            current_rule->add(b);

            // std::cout << "Branch flag " << b.get_recursive_flag() << std::endl;

            b = Branch();
        } else if (token.kind == T_PROB){
            // set this branch's probability
            b.assign_prob(std::stof(token.value));
          
        } else {
            build_branch(b, token);
        }

        consume(1);
    }

    if(curr_token.is_error()){
        std::cout << "[ERROR] " << curr_token.get_error() << std::endl;
    } else if (is_rule_end() && (!b.is_empty())){
        b.set_recursive_flag(current_rule); // set whether or not this branch is non-recursive
        current_rule->add(b);

        // std::cout << "Branch flag " << b.get_recursive_flag() << std::endl;

    }

    // std::cout << "Rule " << current_rule->get_name() << " recursive = " << current_rule->get_recursive_flag() << std::endl;
}

void Grammar::build_grammar(){
    consume(0);

    if(curr_token.is_ok() && (curr_token.get_ok().kind == T_PROB_SET_FLAG)){
        assign_equal_probs = true;
        consume(1);
    }

    Token token;

    while(curr_token.is_ok() && !is_grammar_end()){
        token = curr_token.get_ok();

        rule_pointers[token.value] = get_rule_pointer(token.value);

        consume(1);

        if(curr_token.get_ok().kind == T_EOF){break;}

        consume(T_RULE_START);

        build_rule(rule_pointers[token.value]);
    }

    if(curr_token.is_error()){
        std::cout << "[ERROR] " << curr_token.get_error() << std::endl;
    }

    if(assign_equal_probs){ 
        assign_equal_probabilities();
    }
}

void Grammar::print_grammar() const {

    for(const auto& p : rule_pointers){
        std::cout << p.first << " = ";
        p.second->print(std::cout);
        std::cout << ";" << std::endl;
    }
}

void Grammar::print_rules() const {
    for(const auto& p : rule_pointers){
        std::cout << p.first << " ";
    }
}

void Grammar::assign_equal_probabilities(){

    std::shared_ptr<Rule> ptr;

    for(const auto& p : rule_pointers){
        ptr = p.second;
    
        float prob = 1.0 / (float)ptr->size();

        ptr->assign_prob(prob);
    }
}
