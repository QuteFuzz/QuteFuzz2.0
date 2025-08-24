#include <lex.h>

void Lexer::Lexer::lex(){
    std::string input, matched_string;
    std::vector<Token::Token> tokens;    
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

            } else if (string_is(matched_string, RANGE)){
                tokens.push_back(Token::Token{.kind = Token::RANGE, .value = matched_string});
                
            } else if (string_is(matched_string, LPAREN)){
                tokens.push_back(Token::Token{.kind = Token::LPAREN, .value = matched_string});
                
            } else if (string_is(matched_string, RPAREN)){
                tokens.push_back(Token::Token{.kind = Token::RPAREN, .value = matched_string});

            } else if (string_is(matched_string, OR_EXPAND)){
                size_t len = matched_string.size();
                char lbrack = matched_string.at(0);
                char rbrack = matched_string.at(len - 1);
                std::string str;

                tokens.push_back(Token::Token{.kind = Token::LBRACK, .value = std::string(1, lbrack)});
                
                std::string characters = matched_string.substr(1, len-2); 
                len = characters.size(); // len is now the number of characters inside the brackets

                for(size_t i = 0; i < len - 1; ++i){
                    char c = characters[i];

                    if(c == '-') tokens.push_back(Token::Token{.kind = Token::RANGE, .value = "-"});
                    else {
                        tokens.push_back(Token::Token{.kind = Token::SYNTAX, .value = std::string(1, c)});

                        if(characters[i+1] != '-') tokens.push_back(Token::Token{.kind = Token::SEPARATOR, .value = "|"});
                    }

                }

                tokens.push_back(Token::Token{.kind = Token::SYNTAX, .value = std::string(1, characters[len-1])});
                
                tokens.push_back(Token::Token{.kind = Token::RBRACK, .value = std::string(1, rbrack)});

            } else if (string_is(matched_string, ONE_OR_MORE)){
                tokens.push_back(Token::Token{.kind = Token::ONE_OR_MORE, .value = matched_string});
                
            } else if (string_is(matched_string, ZERO_OR_MORE)){
                tokens.push_back(Token::Token{.kind = Token::ZERO_OR_MORE, .value = matched_string});
                
            } else if (string_is(matched_string, OPTIONAL)){
                tokens.push_back(Token::Token{.kind = Token::OPTIONAL, .value = matched_string});
                
            } else if(string_is(matched_string, ANGLE_RULE)){
                tokens.push_back(Token::Token{.kind = Token::RULE, .value = remove_decorators(matched_string)});

            } else if (string_is(matched_string, RULE)){
                tokens.push_back(Token::Token{.kind = Token::RULE, .value = matched_string});

            } else if (string_is(matched_string, SYNTAX)){

                if(matched_string == "\"\\n\""){
                    tokens.push_back(Token::Token{.kind = Token::SYNTAX, .value = "\n"});
                } else {
                    tokens.push_back(Token::Token{.kind = Token::SYNTAX, .value = remove_decorators(matched_string)});
                }

            } else if (string_is(matched_string, DIGIT)){
                tokens.push_back(Token::Token{.kind = Token::SYNTAX, .value = matched_string});

            } else if (string_is(matched_string, RULE_ENTRY_1) || string_is(matched_string, RULE_ENTRY_2)){
                tokens.push_back(Token::Token{.kind = Token::RULE_START, .value = matched_string});

            } else if (string_is(matched_string, RULE_END)){
                tokens.push_back(Token::Token{.kind = Token::RULE_END, .value = matched_string});

            } else if (string_is(matched_string, SEPARATOR)){
                tokens.push_back(Token::Token{.kind = Token::SEPARATOR, .value = matched_string});

            } 

        }
    }

    tokens.push_back(Token::Token{.kind = Token::_EOF, .value = ""});        
    result.set_ok(tokens);
}

void Lexer::Lexer::print_tokens() const {

    if(result.is_error()){
        ERROR(result.get_error()); 
        
    } else {
        std::vector<Token::Token> tokens = result.get_ok();

        for(size_t i = 0; i < tokens.size(); ++i){
            std::cout << tokens[i] << std::endl;
        }
    }
}