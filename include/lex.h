#ifndef LEX_H
#define LEX_H

#include <regex>
#include <stdio.h>
#include "string.h"
#include <stdlib.h>
#include <fstream>

#include "utils.h"

#define RULE R"([a-zA-Z_\-0-9]+)"
#define DIGIT R"(\-?[0-9]+)"
#define FLOAT DIGIT "(." DIGIT ")?"
#define SYNTAX R"(["'].*?["'])"
#define ANGLE_RULE R"([\<].*?[\>])"
#define RULE_DEF R"(::=)"
#define RULE_ENTRY R"(=)"
#define SEPARATOR R"(\|)"
#define PROB "[" FLOAT "]" 
#define PROB_SET_FLAG R"(equal_prob)"
#define COMMENT R"(;)"

#define FULL_REGEX ("(" PROB_SET_FLAG "|" RULE "|" ANGLE_RULE "|" DIGIT "|" SYNTAX "|" SEPARATOR "|" RULE_DEF "|" RULE_ENTRY "|" PROB "|" COMMENT ")") 

typedef enum {
    T_EOF = 0,
    T_RULE,
    T_SEPARATOR,
    T_RULE_START,
    T_SYNTAX,
    T_PROB,
    T_PROB_SET_FLAG
} Token_kind;

struct Token{
    Token_kind kind;
    std::string value;

    friend std::ostream& operator<<(std::ostream& stream, const Token t){
        switch(t.kind){
            case T_EOF:
                stream << "EOF "; break;
            case T_RULE:
                stream << "RULE "; break;
            case T_RULE_START:
                stream << "RULE START "; break;
            case T_SEPARATOR:
                stream << "RULE SEPARATOR "; break;
            case T_SYNTAX:
                stream << "SYNTAX "; break;
            case T_PROB:
                stream << "PROB "; break;
            case T_PROB_SET_FLAG:
                stream << "PROB SET FLAG "; break;
            default:
                std::cerr << "Unknown token kind" << std::endl;
        }
    
        stream << " " << t.value;
    
        return stream;
    }

};

class Lexer{  
    public:
        Lexer(){}

        Lexer(std::string filename)
            :_filename(filename)
        {
            
            lex();
        }

        std::string remove_decorators(const std::string& token){
            return token.substr(1, token.size()-2);
        }

        void lex(){
            std::string input, matched_string;
            std::vector<Token> tokens;    
            std::ifstream stream(_filename);
            std::regex full_pattern(FULL_REGEX);

            std::sregex_iterator end;

            while(std::getline(stream, input)){
                
                std::sregex_iterator begin(input.begin(), input.end(), full_pattern);

                for(std::sregex_iterator i = begin; i != end; ++i){
                    std::smatch match = *i;
                    matched_string = match.str();

                    if (string_is(matched_string, PROB_SET_FLAG)){
                        tokens.push_back(Token{.kind = T_PROB_SET_FLAG, .value = matched_string});

                    } else if(string_is(matched_string, ANGLE_RULE)){
                        tokens.push_back(Token{.kind = T_RULE, .value = remove_decorators(matched_string)});

                    } else if (string_is(matched_string, RULE)){
                        tokens.push_back(Token{.kind = T_RULE, .value = matched_string});

                    } else if (string_is(matched_string, SYNTAX)){
                        tokens.push_back(Token{.kind = T_SYNTAX, .value = remove_decorators(matched_string)});

                    } else if (string_is(matched_string, RULE_DEF) || string_is(matched_string, RULE_ENTRY)){
                        tokens.push_back(Token{.kind = T_RULE_START, .value = matched_string});

                    } else if (string_is(matched_string, SEPARATOR)){
                        tokens.push_back(Token{.kind = T_SEPARATOR, .value = matched_string});

                    } else if (string_is(matched_string, PROB)){
                        tokens.push_back(Token{.kind = T_PROB, .value = remove_decorators(matched_string)});

                    } else if (string_is(matched_string, COMMENT)){
                        break;
                    }

                }
            }
            
            tokens.push_back(Token{.kind = T_EOF, .value = ""});
        
            result.set_ok(tokens);
            
        }

        void print_tokens(){

            if(result.is_error()){
                std::cout << result.get_error() << std::endl; 
                
            } else {
                std::vector<Token> tokens = result.get_ok();
        
                for(size_t i = 0; i < tokens.size(); ++i){
                    std::cout << tokens[i] << std::endl;
                }
            }
        }

        std::vector<Token> get_tokens(){
            std::vector<Token> tokens = result.get_ok();
            return tokens;
        }

    private:
        Result<std::vector<Token>, std::string> result;
        std::string _filename = "bnf.bnf"; 
        
};

#endif


