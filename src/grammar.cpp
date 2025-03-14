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

    if((current_rule != nullptr) && (token.value == current_rule->get_name())){
        branch.set_recursive_flag();
    }
}

/// @brief The token into a term and add it to all current branches
/// @param tokens 
void Grammar::add_term_to_current_branches(const Token& token){
    for(Branch& current_branch : current_branches){
        add_term_to_branch(token, current_branch);
    }
}

void Grammar::expand_range(){

    if(is_alpha(range_start) && is_alpha(range_end)){
        char from_char = range_start[0];
        char to_char = range_end[0];

        for(char i = from_char + 1; i <= to_char; ++i){
            add_to_expansion_tokens(Token{.kind = TOKEN_SYNTAX, .value = std::string(1, i)});
        }

    } else {
        // assume 2 integers
        int from_int = std::stoi(range_start);
        int to_int = std::stoi(range_end);

        for(int i = from_int + 1; i <= to_int; ++i){
            add_to_expansion_tokens(Token{.kind = TOKEN_SYNTAX, .value = std::to_string(i)});            
        }
    }
}

bool Grammar::is_alpha(const std::string& str){
    return std::all_of(str.begin(), str.end(), ::isalpha);
}

/// @brief Collect all new expansions, create branches for them, and add these branches to the set of current branches
/// @param n 
void Grammar::add_n_branches(const Token& next){

    std::vector<Branch> heads = current_branches;
    expanded_branches_head = heads.size();

    // std::cout << "head p: " << expanded_branches_head << std::endl;
    std::cout << "next " << next << std::endl; 

    int n = 1; // also the default if there's no wildcard token after grouping

    if(next.kind == TOKEN_OPTIONAL){
        n = 1;
    } else if ((next.kind == TOKEN_ONE_OR_MORE) | (next.kind == TOKEN_ZERO_OR_MORE)){
        n = wildcard_max;
    } else {
        drop_heads();
    }

    for(Expansion_option& opt : expansion_tokens){
        
        for(size_t j = 0; j < heads.size(); ++j){
            Branch head = heads[j];

            for(int i = 0; i < n; ++i){

                for(const Token& token : opt){
                    add_term_to_branch(token, head);
                }

                current_branches.push_back(head);
            }
        }
    }

    // expansion_tokens = {{}}; // return to initial state to prepare for new branch

    expansion_tokens.clear();
}

bool Grammar::in_variant_grouping(const Token& current_token){
    peek();

    if(next_token.is_ok()){
        Token token = next_token.get_ok();

        bool explicit_separator = 
            ((token.kind == TOKEN_SEPARATOR) 
            || (prev_token.kind == TOKEN_SEPARATOR) 
            || (current_token.kind == TOKEN_SEPARATOR)) 
            && in_paren;

        return explicit_separator || in_brack;

    } else {
        throw std::runtime_error(next_token.get_error());
    }

    return false;
}

void Grammar::add_to_expansion_tokens(const Token& token){
    Expansion_option option;
    option.push_back(token);
    expansion_tokens.push_back(option);

    std::cout << "added " << token << " as expansion token" << std::endl;
}

void Grammar::build_grammar(){

    if(curr_token.is_ok()){
        Token token = curr_token.get_ok();
        Token next;

        switch(token.kind){
            case TOKEN_RULE : case TOKEN_SYNTAX: {

                if (prev_token.kind == TOKEN_RANGE){
                    range_end = token.value;
                    expand_range();

                } else if(in_variant_grouping(token)){
                    add_to_expansion_tokens(token);

                } else if (just_finished_paren_grouping() || in_paren){
                    std::cout << "added " << token << " to expansion tokens" << std::endl;

                    if(expansion_tokens.size() == 0){
                        expansion_tokens = {Expansion_option{}};
                    }

                    // and this term to all expansion tokens buffers
                    for(Expansion_option& opt : expansion_tokens){
                        opt.push_back(token);
                    }


                } else {
                    add_term_to_current_branches(token);

                }

                break;
            }

            case TOKEN_RULE_START: 
                reset_current_branches();
                current_rule = get_rule_pointer(prev_token.value);
                break;

            case TOKEN_RULE_END: complete_rule(); break;

            case TOKEN_EOF: return;

            case TOKEN_LPAREN: in_paren += 1; break;

            case TOKEN_LBRACK: in_brack += 1; break;

            case TOKEN_RBRACK: in_brack -= 1; break;

            case TOKEN_RPAREN: in_paren -= 1; break;

            case TOKEN_SEPARATOR: {

                if(!in_paren){
                    lazily_add_branches_to_rule();
                    reset_current_branches();
                }

                break;
            }

            case TOKEN_RANGE: 
                range_start = prev_token.value; break;

            case TOKEN_PROB_SET_FLAG: assign_equal_probs = true; break;

            case TOKEN_OPTIONAL: case TOKEN_ZERO_OR_MORE: break;

            case TOKEN_ONE_OR_MORE:
                // drop current heads
                drop_heads();
                break;

            default:
                throw std::runtime_error("[ERROR] Unknown token!"); 
            
        }

        prev_token = token;

        if(can_create_branches()){            
            next = next_token.get_ok();
            add_n_branches(next);
        } 

        consume(1);

        // always peek to prepare for next token
        peek();

        build_grammar();

    } else {
        throw std::runtime_error(curr_token.get_error());
    }
}

void Grammar::print_tokens() const {
    lexer.print_tokens();
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
