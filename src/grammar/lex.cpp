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

            if(string_is(matched_string, R"(#)")){
                break;

            } else if(string_is(matched_string, R"(\(\*)")){
                ignore = true;
                break;
            
            } else if (string_is(matched_string, R"(\*\))")){
                ignore = false;
                break;

            } else {
            
                for(const Token_rule& tr : TOKEN_RULES){
                    if(string_is(matched_string, tr.pattern)){

                        if(tr.kind == Token::SYNTAX){
                            tokens.push_back(Token::Token{remove_outer_quotes(tr.value.value_or(matched_string)), tr.kind});                        
                        
                        } else {
                            tokens.push_back(Token::Token{tr.value.value_or(matched_string), tr.kind});                        
                        }

                        break;
                    }
                }
            }
        }
    }

    tokens.push_back(Token::Token{.value = "", .kind = Token::_EOF});        
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