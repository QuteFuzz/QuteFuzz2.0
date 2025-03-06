#include "../include/grammar.h"

Grammar::Grammar(const fs::path& filename): lexer(filename.string()), name(filename.stem()), path(filename) {
    // lexer.print_tokens();

    tokens = lexer.get_tokens();
    num_tokens = tokens.size();

    consume(0);
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

/// @brief If rule_name hasn't been initialsed, this adds a rule pointer for it and returns that pointer
/// @param rule_name 
/// @return 
std::shared_ptr<Rule> Grammar::get_rule_pointer(const std::string& rule_name){

    if(rule_pointers.find(rule_name) != rule_pointers.end()){
        return rule_pointers[rule_name];
    } else {
        std::shared_ptr<Rule> ptr = std::make_shared<Rule>(rule_name);

        rule_pointers[rule_name] = ptr;

        return ptr;
    }
}

/// @brief Using the given syntax or rule token, create a term and add it to the current branch
/// @param token 
void Grammar::build_branch(const Token& token, Branch& branch){
    peek();

    if(next_token.is_ok() && next_token.get_ok().kind == TOKEN_RULE_START){
        // still before RULE_START TOKEN, set this as the current rule

        if(current_rule != nullptr){
            complete_rule();
        }
        current_rule = get_rule_pointer(token.value);
    } else {

        Term term(token.value);
        
        if(token.kind == TOKEN_SYNTAX){
            term.set_syntax(token.value);
        } else if (token.kind == TOKEN_RULE){
            term.set_pointer(get_rule_pointer(token.value));
        } else {
            throw std::runtime_error("Build branch should only be called on syntax or rule tokens!");
        }

        branch.add(term);
    }
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
            current_branch = Branch();
            build_branch(Token{.kind = TOKEN_SYNTAX, .value = std::string(1, i)}, current_branch);
            current_rule->add(current_branch);
        }

    } else {
        // assume 2 integers
        from_int = std::stoi(from_val);
        to_int = std::stoi(to_val);

        for(int i = from_int; i <= to_int; ++i){
            current_branch = Branch();
            build_branch(Token{.kind = TOKEN_SYNTAX, .value = std::to_string(i)}, current_branch);
            current_rule->add(current_branch);
        }
    }
}

bool Grammar::is_alpha(const std::string& str){
    return std::all_of(str.begin(), str.end(), ::isalpha);
}

void Grammar::add_n_branches(int n){

    for(Expansion_option& opt : expansion_tokens){
        Branch child = current_branch;

        for(int i = 0; i < n; ++i){
            for(Token& token : opt){
                build_branch(token, child);
            }
            // child.print(std::cout);
            current_rule->add(child);
        }
    }
}

void Grammar::expand_group(){
    peek();

    if(next_token.is_ok()){
        Token token = next_token.get_ok();

        if(token.kind == TOKEN_OPTIONAL){
            // add current branch as well as new expansions to the rule 
            current_rule->add(current_branch);
            add_n_branches(1);

        } else if (token.kind == TOKEN_ONE_OR_MORE){
            // add only (new expansions to the rule) * max 
            add_n_branches(wildcard_max);

        } else if (token.kind == TOKEN_ZERO_OR_MORE){
            // add current branch as well as (new expansions to the rule) * max 
            current_rule->add(current_branch);
            add_n_branches(wildcard_max);

        } else {
            // add just the new expasions to the rule
            add_n_branches(1);
        }

        current_branch = Branch();

    } else {
        throw std::runtime_error(next_token.get_error());
    }
}

bool Grammar::in_variant_grouping(const Token& current_token){
    peek();

    if(next_token.is_ok()){
        Token token = next_token.get_ok();

        return ((token.kind == TOKEN_SEPARATOR) || (prev_token.kind == TOKEN_SEPARATOR) || (current_token.kind == TOKEN_SEPARATOR)) && in_grouping;
    } else {
        throw std::runtime_error(next_token.get_error());
    }

    return false;
}

void Grammar::build_grammar(){

    if(curr_token.is_ok()){
        Token token = curr_token.get_ok();

        switch(token.kind){
            case TOKEN_RULE : case TOKEN_SYNTAX: {

                if(in_variant_grouping(token)){
                    Expansion_option option;
                    option.push_back(token);
                    expansion_tokens.push_back(option);
                } else if (just_finished_grouping) {
                    // and this term to all children branches
                    for(Expansion_option& opt : expansion_tokens){
                        opt.push_back(token);
                    }
                } else {
                    build_branch(token, current_branch);

                }

                break;
            }

            case TOKEN_RULE_START: 
                // set the current branch pointer to prepare for rules after this token
                current_branch = Branch(); 
                break;

            case TOKEN_EOF: 

                complete_rule();
                
                return;

            case TOKEN_OPEN_BRACKET: in_grouping += 1; break;

            case TOKEN_CLOSED_BRACKET: {
                in_grouping -= 1;
                
                // collect all child branches, delete all expansion branches
                if(in_grouping == 0){
                    just_finished_grouping = false;
                    expand_group();
                    expansion_tokens.clear();
                } else {
                    just_finished_grouping = true;
                }
                        
                break;
            }

            case TOKEN_SEPARATOR: {

                if(!in_grouping){
                    // need to create new branch
                    current_branch.set_recursive_flag(current_rule); // set whether or not this branch is non-recursive
                    current_rule->add(current_branch);
                    current_branch = Branch();
                }
                break;
            }

            case TOKEN_PROB: current_branch.assign_prob(std::stof(token.value)); break;

            case TOKEN_RANGE: {
                peek();

                if (next_token.is_error()){
                    std::cout << next_token.get_error() << std::endl;
                } else {
                    expand_range(current_rule, prev_token, next_token.get_ok());
                    consume(1);

                    current_branch = Branch();
                }

                break;
            }

            case TOKEN_PROB_SET_FLAG: assign_equal_probs = true; break;

            case TOKEN_OPTIONAL: {
                break;
            }

            case TOKEN_ONE_OR_MORE: break;

            case TOKEN_ZERO_OR_MORE: break;
            
            default:
                throw std::runtime_error("[ERROR] Unknown token!"); 
            
        }

        prev_token = token;

        consume(1);
        build_grammar();

    } else {
        throw std::runtime_error(curr_token.get_error());
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

/// @brief Assigns all branches in the current rule equal probabilites
void Grammar::assign_equal_probabilities(){
    float prob = 1.0 / (float)current_rule->size();

    // std::cout << current_rule->get_name() << " " << prob << std::endl;

    current_rule->assign_prob(prob);
}
