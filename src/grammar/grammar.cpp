#include <grammar.h>

Grammar::Grammar(const fs::path& filename, std::vector<Token::Token>& meta_grammar_tokens): lexer(filename.string()), name(filename.stem()), path(filename) {
    // remove EOF from meta grammar's tokens
    meta_grammar_tokens.pop_back();
    
    tokens = append_vectors(meta_grammar_tokens, lexer.get_tokens());

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
std::shared_ptr<Rule> Grammar::get_rule_pointer(std::string rule_name, U8 scope){
    auto dummy = std::make_shared<Rule>(rule_name, scope);

    for(const auto& rp : rule_pointers){
        if(*rp == *dummy){return rp;}
    }

    rule_pointers.push_back(dummy);

    return dummy;
}

/// @brief Convert a single token into a term and add it to the given branch
/// @param token 
void Grammar::add_term_to_branch(const Token::Token& token, Branch& branch){

    Term term(token.value, nesting_depth);
    
    if(token.kind == Token::SYNTAX){
        term.set(token.value);

    } else if (token.kind == Token::RULE){
        /*
            each term within the branch of a rule has a scope associated with it
            if explicitly specified like EXTERNAL::term, then the term takes on that scope, otherwise, it 
            takes on the scope of the current rule (i.e the rule def)
        */
        U8 scope = (rule_decl_scope == NO_SCOPE) && (current_rule != nullptr) ? current_rule->get_scope() : rule_decl_scope;
    
        term.set(get_rule_pointer(token.value, scope));

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

                auto terms = append_vectors(basis.remainders, multiply_vector(basis.mults, mult));
                
                Branch extension(terms);
                extensions.push_back(extension);
            }

        }
    }

    increment_nesting_depth_base(); // must be done after wildcards are processed

    current_branches.insert(current_branches.end(), extensions.begin(), extensions.end());
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

                // rules that are within branches, rules before `RULE_START` are handled at `RULE_START`
                if(current_rule != nullptr){
                    add_term_to_current_branches(token);
                    rule_decl_scope = NO_SCOPE;
                }

                break;
            }

            case Token::RULE_START: {
                reset_current_branches();
                
                current_rule = get_rule_pointer(prev_token.value, rule_def_scope);
                break;
            }

            case Token::RULE_END: complete_rule(); current_rule = nullptr; break;

            case Token::_EOF: return; // should never peek if current token was EOF

            case Token::LPAREN: case Token::LBRACK: nesting_depth += 1; break;

            case Token::RBRACK: case Token::RPAREN: 
                nesting_depth -= 1; 

                next = next_token.get_ok();

                if(!is_wilcard(next)){
                    increment_nesting_depth_base();
                }

                break;

            case Token::SEPARATOR:
                add_current_branches_to_rule();
                reset_current_branches();
                break;

            case Token::OPTIONAL: case Token::ZERO_OR_MORE: case Token::ONE_OR_MORE:
                extend_current_branches(token);
                break;

            case Token::LBRACE: 
                break;
            
            case Token::RBRACE: rule_def_scope = NO_SCOPE; break;

            case Token::EXTERNAL:
                if(current_rule == nullptr){
                    rule_def_scope |= EXTERNAL_SCOPE;
                } else {
                    rule_decl_scope |= EXTERNAL_SCOPE;
                }

                break;
                        
            case Token::INTERNAL:
                if(current_rule == nullptr){
                    rule_def_scope |= INTERNAL_SCOPE;
                } else {
                    rule_decl_scope |= INTERNAL_SCOPE;
                }
                
                break;
            
            case Token::OWNED: 
                if(current_rule == nullptr){
                    rule_def_scope |= OWNED_SCOPE;
                } else {
                    rule_decl_scope |= OWNED_SCOPE;
                }

                break;

            case Token::ARROW: break;

            case Token::COMMENT: case Token::MULTI_COMMENT_START: case Token::MULTI_COMMENT_END: break;

            default:
                throw std::runtime_error(ANNOT("Unknown token: " + token.value)); 
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

void Grammar::print_rules() const {
    for(const auto& p : rule_pointers){
        std::cout << p->get_name() << " ";
    }
}
