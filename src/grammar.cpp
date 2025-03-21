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

    Term term(token.value, nesting_depth);
    
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
    if(current_branches.size() == 0){
        Branch b;
        add_term_to_branch(token, b);
        current_branches.push_back(b);    
    } else {
        for(Branch& current_branch : current_branches){
            add_term_to_branch(token, current_branch);
        }
    }

}

void Grammar::extend_current_branches(const Token& wildcard){
    // loop through current heads, and multiply
    std::vector<Branch> extensions;
    Branch_multiply basis;

    for(const Branch& current_branch : current_branches){
        if(!current_branch.is_empty()){
            basis.clear();
            current_branch.setup_basis(basis, nesting_depth);

            if(wildcard.kind == TOKEN_OPTIONAL){
                extensions.push_back(Branch(basis.remainders));
                break;            

            } else if (wildcard.kind == TOKEN_ZERO_OR_MORE){
                extensions.push_back(Branch(basis.remainders));
            }
        
            // use basis to get extensions depending on the wildcard being processed
            for(unsigned int mult = 2; mult <= WILDCARD_MAX; ++mult){
                auto terms = append_vectors(basis.remainders, multiply_vector(basis.mults, mult));
                Branch extension(terms);
                extensions.push_back(extension);
            }

        }
    }

    current_branches.insert(current_branches.end(), extensions.begin(), extensions.end());
}

void Grammar::build_grammar(){

    if(curr_token.is_ok()){
        Token token = curr_token.get_ok();

        // cannot set here because if curr token is EOF, next should be an error. 
        // I set this for only the specific cases where I use it to avoid an extra if statement checking for ok here
        Token next;

        switch(token.kind){
            case TOKEN_RULE : case TOKEN_SYNTAX: {
                // next = next_token.get_ok();

                add_term_to_current_branches(token);
                break;
            }

            case TOKEN_RULE_START: 
                reset_current_branches();
                current_rule = get_rule_pointer(prev_token.value);
                break;

            case TOKEN_RULE_END: complete_rule(); break;

            case TOKEN_EOF: return; // should never peek if current token was EOF

            case TOKEN_LPAREN: case TOKEN_LBRACK: nesting_depth += 1; break;

            case TOKEN_RBRACK: case TOKEN_RPAREN: nesting_depth -= 1; break;

            case TOKEN_SEPARATOR: {

                add_current_branches_to_rule();
                reset_current_branches();

                break;
            }

            case TOKEN_RANGE: 
                range_start = prev_token.value; 
                range_end = next_token.get_ok().value;
                

                break;

            case TOKEN_PROB_SET_FLAG: assign_equal_probs = true; break;

            case TOKEN_OPTIONAL: case TOKEN_ZERO_OR_MORE: case TOKEN_ONE_OR_MORE:
                extend_current_branches(token);
                break;

            default:
                throw std::runtime_error("[ERROR] Unknown token!"); 
            
        }

        prev_token = token;

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
