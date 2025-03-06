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

void Grammar::previous(){
    if(token_pointer == 0){
        prev_token.set_error("Cannot find previous token from here!");
    } else {
        prev_token.set_ok(tokens[token_pointer - 1]);
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
    
    if(token.kind == TOKEN_SYNTAX){
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
        return (next_token.get_ok().kind == TOKEN_RULE_START);
    }
}

bool Grammar::is_grammar_end(){

    return (curr_token.get_ok().kind == TOKEN_EOF);
}

void Grammar::expand_range(std::shared_ptr<Rule> current_rule, const Token& from, const Token& to){
    // std::cout << "alpha: " << from.value << is_alpha(from.value) << std::endl;

    std::string from_val = from.value, to_val = to.value;

    int from_int, to_int;
    char from_char, to_char;

    if(is_alpha(from_val) && is_alpha(to_val)){
        from_char = from_val[0];
        to_char = to_val[0];

        for(char i = from_char; i <= to_char; ++i){
            Branch branch;
            build_branch(branch, Token{.kind = TOKEN_SYNTAX, .value = std::string(1, i)});
            current_rule->add(branch);
        }

    } else {
        // assume 2 integers
        from_int = std::stoi(from_val);
        to_int = std::stoi(to_val);

        for(int i = from_int; i <= to_int; ++i){
            Branch branch;
            build_branch(branch, Token{.kind = TOKEN_SYNTAX, .value = std::to_string(i)});
            current_rule->add(branch);
        }
    }
}

bool Grammar::is_alpha(const std::string& str){
    return std::all_of(str.begin(), str.end(), ::isalpha);
}

void Grammar::build_rule(std::shared_ptr<Rule> current_rule){
    Branch b;
    Token token;

    while(curr_token.is_ok() && !is_rule_end()){
        token = curr_token.get_ok();

        if(token.kind == TOKEN_SEPARATOR){
            // need to create new branch
            b.set_recursive_flag(current_rule); // set whether or not this branch is non-recursive
            current_rule->add(b);

            b = Branch();
        } else if (token.kind == TOKEN_PROB){
            // set this branch's probability
            b.assign_prob(std::stof(token.value));
          
        } else if (token.kind == TOKEN_RANGE){
            previous();
            peek();

            if(prev_token.is_error()){
                std::cout << prev_token.get_error() << std::endl;
            } else if (next_token.is_error()){
                std::cout << prev_token.get_error() << std::endl;
            } else {
                expand_range(current_rule, prev_token.get_ok(), next_token.get_ok());
                consume(1);

                b = Branch();
            }

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
    }
}

void Grammar::build_grammar(){
    consume(0);

    if(curr_token.is_ok() && (curr_token.get_ok().kind == TOKEN_PROB_SET_FLAG)){
        assign_equal_probs = true;
        consume(1);
    }

    Token token;

    while(curr_token.is_ok() && !is_grammar_end()){
        token = curr_token.get_ok();

        rule_pointers[token.value] = get_rule_pointer(token.value);

        consume(1);

        if(curr_token.get_ok().kind == TOKEN_EOF){break;}

        consume(TOKEN_RULE_START);

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


