#include <grammar.h>

template<typename T>
std::vector<T> multiply_vector(std::vector<T> vec, int mult){
    std::vector<T> multiplied_vec;
    
    multiplied_vec.reserve(vec.size() * mult);

    for(int i = 0; i < mult; ++i){
        multiplied_vec.insert(multiplied_vec.end(), vec.begin(), vec.end());
    }

    return multiplied_vec;
}

template<typename T>
std::vector<T> append_vectors(std::vector<T> vec1, std::vector<T> vec2){
    std::vector<T> result = vec1;

    result.insert(result.end(), vec2.begin(), vec2.end());

    return result;
}

Grammar::Grammar(const fs::path& filename): lexer(filename.string()), name(filename.stem()), path(filename) {
    // lexer.print_tokens();

    tokens = lexer.get_tokens();
    num_tokens = tokens.size();

    consume(0); // prepare current token
    peek(); // prepare next token
}

void Grammar::consume(int n){

    token_pointer += n;
            
    if(token_pointer == num_tokens){
        curr_token.set_error("Out of tokens!");
    } else {
        curr_token.set_ok(tokens[token_pointer]);
    }
}

void Grammar::consume(const Token::Token_kind kind){

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
std::shared_ptr<Rule> Grammar::get_rule_pointer(std::string rule_name){

    // lower(rule_name);

    if(rule_pointers.find(rule_name) == rule_pointers.end()){
        std::shared_ptr<Rule> ptr = std::make_shared<Rule>(rule_name);
        rule_pointers[rule_name] = ptr;
    }

    return rule_pointers[rule_name];
}

/// @brief Convert a single token into a term and add it to the given branch
/// @param token 
void Grammar::add_term_to_branch(const Token::Token& token, Branch& branch){

    Term term(token.value, nesting_depth);
    
    if(token.kind == Token::SYNTAX){
        term.set(token.value);
    } else if (token.kind == Token::RULE){
        term.set(get_rule_pointer(token.value));
    } else {
        throw std::runtime_error(ANNOT("Build branch should only be called on syntax or rule tokens!"));
    }

    branch.add(term);

    if((current_rule != nullptr) && (token.value == current_rule->get_name()) && (token.kind == Token::RULE)){
        branch.set_recursive_flag();
    }
}

/// @brief The token into a term and add it to all current branches
/// @param tokens 
void Grammar::add_term_to_current_branches(const Token::Token& token){
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

void Grammar::extend_current_branches(const Token::Token& wildcard){
    // loop through current heads, and multiply
    std::vector<Branch> extensions;
    Branch_multiply basis;

    for(const Branch& current_branch : current_branches){
        if(!current_branch.is_empty()){
            basis.clear();
            current_branch.setup_basis(basis, nesting_depth);

            if(wildcard.kind == Token::OPTIONAL){
                extensions.push_back(Branch(basis.remainders));
                break;            

            } else if (wildcard.kind == Token::ZERO_OR_MORE){
                extensions.push_back(Branch(basis.remainders));
            }
        
            // use basis to get extensions depending on the wildcard being processed
            for(unsigned int mult = 2; mult <= WILDCARD_MAX; ++mult){

                auto terms = append_vectors(
                                basis.remainders, 
                                multiply_vector(basis.mults, mult)
                        );
                Branch extension(terms);
                extensions.push_back(extension);
            }

        }
    }

    increment_nesting_depth_base(); // must be done after wildcards are processed

    current_branches.insert(current_branches.end(), extensions.begin(), extensions.end());
}

void Grammar::expand_range(){
    char begin = range_start[0];
    char end = range_end[0];

    std::vector<Branch> new_current_branches;
    
    if(current_branches.size() == 0){
        current_branches.assign(1, Branch());
    }

    for(char i = begin; i <= end; ++i){
        Token::Token token = {.kind = Token::SYNTAX, .value = std::string(1, i)};
        
        for(Branch copy : current_branches){
            add_term_to_branch(token, copy);
            new_current_branches.push_back(copy);
        }
    }

    current_branches = new_current_branches;
}

void Grammar::build_grammar(){

    if(curr_token.is_ok()){
        Token::Token token = curr_token.get_ok();

        // cannot set here because if curr token is EOF, next should be an error. 
        // I set this for only the specific cases where I use it to avoid an extra if statement checking for ok here
        Token::Token next;

        switch(token.kind){
            case Token::RULE : case Token::SYNTAX: {

                next = next_token.get_ok();

                if((next.kind != Token::RANGE) && (prev_token.kind != Token::RANGE)){
                    add_term_to_current_branches(token);
                }

                break;
            }

            case Token::RULE_START: 
                reset_current_branches();
                current_rule = get_rule_pointer(prev_token.value);
                break;

            case Token::RULE_END: complete_rule(); break;

            case Token::_EOF: return; // should never peek if current token was EOF

            case Token::LPAREN: case Token::LBRACK: nesting_depth += 1; break;

            case Token::RBRACK: case Token::RPAREN: 
                nesting_depth -= 1; 

                next = next_token.get_ok();

                if(!is_wilcard(next)){
                    increment_nesting_depth_base();
                }

                break;

            case Token::SEPARATOR: {

                add_current_branches_to_rule();
                reset_current_branches();

                break;
            }

            case Token::RANGE: 
                range_start = prev_token.value; 
                range_end = next_token.get_ok().value;
                
                expand_range();

                break;

            case Token::OPTIONAL: case Token::ZERO_OR_MORE: case Token::ONE_OR_MORE:
                extend_current_branches(token);
                break;

            case Token::LBRACE: case Token::RBRACE: break;

            default:
                throw std::runtime_error(ANNOT("Unknown token!")); 
            
        }

        prev_token = token;

        consume(1);

        // always peek to prepare for next token
        peek();

        build_grammar();

    } else {
        ERROR(curr_token.get_error());
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
