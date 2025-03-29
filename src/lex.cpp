#include "../include/lex.h"

void Lexer::lex(){
    std::string input, matched_string;
    std::vector<Token> tokens;    
    std::ifstream stream(_filename);
    std::regex full_pattern(FULL_REGEX);

    std::sregex_iterator end;

    while(std::getline(stream, input)){
        
        std::sregex_iterator begin(input.begin(), input.end(), full_pattern);

        for(std::sregex_iterator i = begin; (i != end); ++i){
            std::smatch match = *i;
            matched_string = match.str();

            if (string_is(matched_string, MULTI_COMMENT_START)){
                ignore = true;
                
            } else if (string_is(matched_string, MULTI_COMMENT_END)){
                ignore = false;

            } else if (string_is(matched_string, COMMENT)){
                break;

            } else if (string_is(matched_string, PROB_SET_FLAG)){
                tokens.push_back(Token{.kind = TOKEN_PROB_SET_FLAG, .value = matched_string});

            } else if (string_is(matched_string, RANGE)){
                tokens.push_back(Token{.kind = TOKEN_RANGE, .value = matched_string});
                
            } else if (string_is(matched_string, LPAREN)){
                tokens.push_back(Token{.kind = TOKEN_LPAREN, .value = matched_string});
                
            } else if (string_is(matched_string, RPAREN)){
                tokens.push_back(Token{.kind = TOKEN_RPAREN, .value = matched_string});

            } else if (string_is(matched_string, OR_EXPAND)){
                size_t len = matched_string.size();
                char lbrack = matched_string.at(0);
                char rbrack = matched_string.at(len - 1);
                std::string str;

                tokens.push_back(Token{.kind = TOKEN_LBRACK, .value = std::string(1, lbrack)});
                
                std::string characters = matched_string.substr(1, len-2); 
                len = characters.size(); // len is now the number of characters inside the brackets

                for(size_t i = 0; i < len - 1; ++i){
                    char c = characters[i];

                    if(c == '-') tokens.push_back(Token{.kind = TOKEN_RANGE, .value = "-"});
                    else {
                        tokens.push_back(Token{.kind = TOKEN_SYNTAX, .value = std::string(1, c)});

                        if(characters[i+1] != '-') tokens.push_back(Token{.kind = TOKEN_SEPARATOR, .value = "|"});
                    }

                }

                tokens.push_back(Token{.kind = TOKEN_SYNTAX, .value = std::string(1, characters[len-1])});
                
                tokens.push_back(Token{.kind = TOKEN_RBRACK, .value = std::string(1, rbrack)});

            } else if (string_is(matched_string, ONE_OR_MORE)){
                tokens.push_back(Token{.kind = TOKEN_ONE_OR_MORE, .value = matched_string});
                
            } else if (string_is(matched_string, ZERO_OR_MORE)){
                tokens.push_back(Token{.kind = TOKEN_ZERO_OR_MORE, .value = matched_string});
                
            } else if (string_is(matched_string, OPTIONAL)){
                tokens.push_back(Token{.kind = TOKEN_OPTIONAL, .value = matched_string});
                
            } else if(string_is(matched_string, ANGLE_RULE)){
                tokens.push_back(Token{.kind = TOKEN_RULE, .value = remove_decorators(matched_string)});

            } else if (string_is(matched_string, RULE)){
                tokens.push_back(Token{.kind = TOKEN_RULE, .value = matched_string});

            } else if (string_is(matched_string, SYNTAX)){
                tokens.push_back(Token{.kind = TOKEN_SYNTAX, .value = remove_decorators(matched_string)});

            } else if (string_is(matched_string, DIGIT)){
                tokens.push_back(Token{.kind = TOKEN_SYNTAX, .value = matched_string});

            } else if (string_is(matched_string, RULE_ENTRY_1) || string_is(matched_string, RULE_ENTRY_2)){
                tokens.push_back(Token{.kind = TOKEN_RULE_START, .value = matched_string});

            } else if (string_is(matched_string, RULE_END)){
                tokens.push_back(Token{.kind = TOKEN_RULE_END, .value = matched_string});

            } else if (string_is(matched_string, SEPARATOR)){
                tokens.push_back(Token{.kind = TOKEN_SEPARATOR, .value = matched_string});

            } 

        }
    }

    tokens.push_back(Token{.kind = TOKEN_EOF, .value = ""});        
    result.set_ok(tokens);
}

void Lexer::print_tokens() const {

    if(result.is_error()){
        std::cout << result.get_error() << std::endl; 
        
    } else {
        std::vector<Token> tokens = result.get_ok();

        for(size_t i = 0; i < tokens.size(); ++i){
            std::cout << tokens[i] << std::endl;
        }
    }
}