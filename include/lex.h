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
#define RULE_ENTRY_0 R"(::=)"
#define RULE_ENTRY_1 R"(=)"
#define RULE_ENTRY_2 R"(:)"
#define SEPARATOR R"(\|)"
#define PROB "[" FLOAT "]" 
#define PROB_SET_FLAG R"(equal_prob)"
#define COMMENT R"(//)"
#define MULTI_COMMENT_START R"(/\*)"
#define MULTI_COMMENT_END R"(\*/)"
#define RULE_END R"(;)"
#define RANGE R"(\-)"
#define OPEN_BRACKET R"(\()"
#define CLOSED_BRACKET R"(\))"
#define ZERO_OR_MORE R"(\*)"
#define OPTIONAL R"(\?)"
#define ONE_OR_MORE R"(\+)"

#define FULL_REGEX "(" PROB_SET_FLAG "|" RULE "|" ANGLE_RULE "|" DIGIT "|" SYNTAX "|" SEPARATOR "|" \
        RULE_ENTRY_0 "|" RULE_ENTRY_1 "|" RULE_ENTRY_2 "|" PROB "|" COMMENT "|" MULTI_COMMENT_START "|" \
        MULTI_COMMENT_END "|" RULE_END "|" RANGE "|" OPEN_BRACKET "|" CLOSED_BRACKET "|" ZERO_OR_MORE "|" \
        OPTIONAL "|" ONE_OR_MORE \
        ")"
        
typedef enum {
    TOKEN_EOF = 0,
    TOKEN_RULE,
    TOKEN_SEPARATOR,
    TOKEN_RULE_START,
    TOKEN_RULE_END,
    TOKEN_SYNTAX,
    TOKEN_PROB,
    TOKEN_PROB_SET_FLAG,
    TOKEN_RANGE,
    TOKEN_OPEN_BRACKET,
    TOKEN_CLOSED_BRACKET,
    TOKEN_ZERO_OR_MORE,
    TOKEN_ONE_OR_MORE,
    TOKEN_OPTIONAL
} Token_kind;

struct Token{
    Token_kind kind;
    std::string value;

    friend std::ostream& operator<<(std::ostream& stream, const Token t){
        switch(t.kind){
            case TOKEN_EOF:
                stream << "EOF "; break;
            case TOKEN_RULE:
                stream << "RULE "; break;
            case TOKEN_RULE_START:
                stream << "RULE START "; break;
            case TOKEN_RULE_END:
                stream << "RULE END "; break;
            case TOKEN_SEPARATOR:
                stream << "SEPARATOR "; break;
            case TOKEN_SYNTAX:
                stream << "SYNTAX "; break;
            case TOKEN_PROB:
                stream << "PROB "; break;
            case TOKEN_PROB_SET_FLAG:
                stream << "PROB SET FLAG "; break;
            case TOKEN_RANGE:
                stream << "RANGE "; break;
            case TOKEN_OPEN_BRACKET:
                stream << "OPEN BRACKET "; break;
            case TOKEN_CLOSED_BRACKET:
                stream << "CLOSED BRACKET "; break;
            case TOKEN_ZERO_OR_MORE:
                stream << "ZERO OR MORE "; break;
            case TOKEN_ONE_OR_MORE:
                stream << "ONE OR MORE "; break;
            case TOKEN_OPTIONAL:
                stream << "OPTIONAL "; break;
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

        inline bool string_is(const std::string& string, const std::string& pattern){

            return std::regex_match(string, std::regex(pattern)) && ((ignore == false) || (pattern == MULTI_COMMENT_END));
        }

        void lex();

        void print_tokens();

        std::vector<Token> get_tokens(){
            std::vector<Token> tokens = result.get_ok();
            return tokens;
        }

    private:
        Result<std::vector<Token>, std::string> result;
        std::string _filename = "bnf.bnf"; 
        bool ignore = false;
        
};

#endif


