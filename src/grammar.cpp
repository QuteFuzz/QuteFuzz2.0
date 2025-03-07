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

/// @brief Convert a single token into a term and add it to the given branch
/// @param token 
void Grammar::add_term_to_branch(const Token& token, Branch& branch){

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

/// @brief Convert each token into a term and add it to the given branch
/// @param tokens 
/// @param branch 
void Grammar::add_terms_to_branch(const std::vector<Token>& tokens, Branch& branch){

    for(size_t i = 0; i < tokens.size(); ++i){
        add_term_to_branch(tokens[i], branch);
    }
}

/// @brief Convert each token into a term and add it to all current branches
/// @param tokens 
void Grammar::add_terms_to_current_branches(const std::vector<Token>& tokens){
    for(Branch& current_branch : current_branches){
        add_terms_to_branch(tokens, current_branch);
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

        for(char i = from_char + 1; i <= to_char; ++i){
            Branch b;
            add_term_to_branch(Token{.kind = TOKEN_SYNTAX, .value = std::string(1, i)}, b);
            current_branches.push_back(b);
        }

    } else {
        // assume 2 integers
        from_int = std::stoi(from_val);
        to_int = std::stoi(to_val);

        for(int i = from_int + 1; i <= to_int; ++i){
            Branch b;
            add_term_to_branch(Token{.kind = TOKEN_SYNTAX, .value = std::to_string(i)}, b);
            current_branches.push_back(b);
        }
    }
}

bool Grammar::is_alpha(const std::string& str){
    return std::all_of(str.begin(), str.end(), ::isalpha);
}

/// @brief Collect all new expansions, create branches for them, and add these branches to the current rule
/// make current branch a new branch
/// @param n 
void Grammar::add_n_branches(const Token& next){

    int n = 1; // also the default if there's no wildcard token after grouping

    if(next.kind == TOKEN_OPTIONAL){
        n = 1;
    } else if ((next.kind == TOKEN_ONE_OR_MORE) | (next.kind == TOKEN_ZERO_OR_MORE)){
        n = wildcard_max;
    }

    std::vector<Branch> heads = current_branches;
    expanded_branches_head = heads.size();

    for(Expansion_option& opt : expansion_tokens){

        for(size_t j = 0; j < heads.size(); ++j){
            Branch head = heads[j];

            for(int i = 0; i < n; ++i){
                add_terms_to_branch(opt, head);
                head.set_recursive_flag(current_rule);
                current_branches.push_back(head);
            }
        }
    }

    expansion_tokens.clear();
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

                } else if (just_finished_grouping || in_grouping) {
                    if(in_grouping && (expansion_tokens.size() == 0)){
                        expansion_tokens.push_back({});
                    }

                    // and this term to all children branches
                    for(Expansion_option& opt : expansion_tokens){
                        opt.push_back(token);
                    }

                } else {
                    add_terms_to_current_branches({token});

                }

                break;
            }

            case TOKEN_RULE_START: 
                reset_current_branches();
                current_rule = get_rule_pointer(prev_token.value);
                break;

            case TOKEN_RULE_END: 
                complete_rule();
                break;

            case TOKEN_EOF: return;

            case TOKEN_OPEN_BRACKET: in_grouping += 1; break;

            case TOKEN_CLOSED_BRACKET: {
                in_grouping -= 1;
                
                if(in_grouping == 0){
                    just_finished_grouping = false;

                    peek();

                    if(next_token.is_ok()){
                        Token next = next_token.get_ok();
                        add_n_branches(next);
                    }

                } else {
                    just_finished_grouping = true;
                }
                        
                break;
            }

            case TOKEN_SEPARATOR: {

                if(!in_grouping){
                    lazily_add_branches_to_rule();
                    reset_current_branches();
                }

                break;
            }

            case TOKEN_PROB: break;

            case TOKEN_RANGE: {
                peek();

                if (next_token.is_error()){
                    std::cout << next_token.get_error() << std::endl;
                } else {
                    expand_range(current_rule, prev_token, next_token.get_ok());
                    consume(1);

                }

                break;
            }

            case TOKEN_PROB_SET_FLAG: assign_equal_probs = true; break;

            case TOKEN_OPTIONAL: case TOKEN_ZERO_OR_MORE: break;

            case TOKEN_ONE_OR_MORE:

                // for(Branch b : current_branches){
                //     b.print(std::cout);
                //     std::cout << std::endl;
                // }

                // std::cout << expanded_branches_head << std::endl;

                // drop current heads
                current_branches = std::vector<Branch>(current_branches.begin() + expanded_branches_head, current_branches.end());

                break;

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
