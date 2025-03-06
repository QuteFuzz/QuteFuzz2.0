#include "../include/lex.h"

void Lexer::lex(){
    std::string input, matched_string;
    std::vector<Token> tokens;    
    std::ifstream stream(_filename);
    std::regex full_pattern(FULL_REGEX);

    std::sregex_iterator end;

    while(std::getline(stream, input)){
        
        std::sregex_iterator begin(input.begin(), input.end(), full_pattern);

        for(std::sregex_iterator i = begin; (i != end) && (ignore == false); ++i){
            std::smatch match = *i;
            matched_string = match.str();

            if (string_is(matched_string, MULTI_COMMENT)){
                ignore = !ignore;
                
            } else if (string_is(matched_string, COMMENT)){
                break;

            } else if (string_is(matched_string, PROB_SET_FLAG)){
                tokens.push_back(Token{.kind = TOKEN_PROB_SET_FLAG, .value = matched_string});

            } else if (string_is(matched_string, RANGE)){
                tokens.push_back(Token{.kind = TOKEN_RANGE, .value = matched_string});
                
            } else if (string_is(matched_string, OPEN_BRACKET)){
                tokens.push_back(Token{.kind = TOKEN_OPEN_BRACKET, .value = matched_string});
                
            } else if (string_is(matched_string, CLOSED_BRACKET)){
                tokens.push_back(Token{.kind = TOKEN_CLOSED_BRACKET, .value = matched_string});
                
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

            } else if (string_is(matched_string, RULE_DEF) || string_is(matched_string, RULE_ENTRY)){
                tokens.push_back(Token{.kind = TOKEN_RULE_START, .value = matched_string});

            } else if (string_is(matched_string, SEPARATOR)){
                tokens.push_back(Token{.kind = TOKEN_SEPARATOR, .value = matched_string});

            } else if (string_is(matched_string, PROB)){
                tokens.push_back(Token{.kind = TOKEN_PROB, .value = remove_decorators(matched_string)});
            } 

        }
    }

    tokens.push_back(Token{.kind = TOKEN_EOF, .value = ""});        
    result.set_ok(tokens);
}

void Lexer::print_tokens(){

    if(result.is_error()){
        std::cout << result.get_error() << std::endl; 
        
    } else {
        std::vector<Token> tokens = result.get_ok();

        for(size_t i = 0; i < tokens.size(); ++i){
            std::cout << tokens[i] << std::endl;
        }
    }
}