#ifndef LEX_H
#define LEX_H

#include <regex>
#include <stdio.h>
#include "string.h"
#include <stdlib.h>
#include <fstream>

#include "utils.h"

namespace Token {
    typedef enum {
        _EOF = 0,
        RULE,
        SEPARATOR,
        RULE_START,
        RULE_END,
        SYNTAX,
        PROB_SET_FLAG,
        RANGE,
        LPAREN,
        LBRACK,
        LBRACE,
        RPAREN,
        RBRACK,
        RBRACE,
        ZERO_OR_MORE,
        ONE_OR_MORE,
        OPTIONAL
    } Token_kind;

    struct Token{
        Token_kind kind;
        std::string value;

        friend std::ostream& operator<<(std::ostream& stream, const Token t){
            switch(t.kind){
                case _EOF:
                    stream << "EOF "; break;
                case RULE:
                    stream << "RULE "; break;
                case RULE_START:
                    stream << "RULE START "; break;
                case RULE_END:
                    stream << "RULE END "; break;
                case SEPARATOR:
                    stream << "SEPARATOR "; break;
                case SYNTAX:
                    stream << "SYNTAX "; break;
                case PROB_SET_FLAG:
                    stream << "PROB SET FLAG "; break;
                case RANGE:
                    stream << "RANGE "; break;
                case LPAREN:
                    stream << "LPAREN "; break;
                case RPAREN:
                    stream << "RPAREN "; break;
                case LBRACE:
                    stream << "LBRACE "; break;
                case RBRACE:
                    stream << "RBRACE "; break;
                case LBRACK:
                    stream << "LBRACK "; break;
                case RBRACK:
                    stream << "RBRACK "; break;
                case ZERO_OR_MORE:
                    stream << "ZERO OR MORE "; break;
                case ONE_OR_MORE:
                    stream << "ONE OR MORE "; break;
                case OPTIONAL:
                    stream << "OPTIONAL "; break;
                default:
                    std::cerr << "Unknown token kind" << std::endl;
            }
        
            stream << " " << t.value;
        
            return stream;
        }
    };
}

namespace Lexer {

    const std::string CHAR = R"([a-zA-Z_])";
    const std::string RULE = R"([a-zA-Z_1-9]+)";
    const std::string DIGIT = R"([0-9])";
    const std::string SYNTAX = R"(["'].*?["'])";
    const std::string ANGLE_RULE = R"([\<].*?[\>])";
    const std::string RULE_ENTRY_1 = R"(=)";
    const std::string RULE_ENTRY_2 = R"(:)";
    const std::string SEPARATOR = R"(\|)";
    const std::string PROB_SET_FLAG = R"(equal_prob)";
    const std::string COMMENT = R"(#)";
    const std::string MULTI_COMMENT_START = R"(\(\*)";
    const std::string MULTI_COMMENT_END = R"(\*\))";
    const std::string RULE_END = R"(;)";
    const std::string RANGE = R"(\-)";
    const std::string LPAREN = R"(\()";
    const std::string RPAREN = R"(\))";
    const std::string LBRACK = R"(\[)";
    const std::string RBRACK = R"(\])";
    const std::string LBRACE = R"(\{)";
    const std::string RBRACE = R"(\})";
    const std::string ZERO_OR_MORE = R"(\*)";
    const std::string OPTIONAL = R"(\?)";
    const std::string ONE_OR_MORE = R"(\+)";

    const std::string OR_EXPAND = "(" + LBRACK + ".*?" + RBRACK + ")";

    const std::string FULL_REGEX = "(" + PROB_SET_FLAG + "|" + OR_EXPAND + "|" +  RULE + "|" + ANGLE_RULE + "|" + DIGIT + "|" + SYNTAX + "|" + SEPARATOR + "|" + \
            RULE_ENTRY_1 + "|" + RULE_ENTRY_2 + "|" + COMMENT + "|" + MULTI_COMMENT_START + "|" + \
            MULTI_COMMENT_END + "|" + RULE_END + "|" + RANGE + "|" + LPAREN + "|" + RPAREN + "|" + \
            LBRACK + "|" + RBRACK + "|" + LBRACE + "|" + RBRACE + "|" + \
            ZERO_OR_MORE + "|" + OPTIONAL + "|" + ONE_OR_MORE + \
            ")";
            
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

            void print_tokens() const;

            std::vector<Token::Token> get_tokens(){
                std::vector<Token::Token> tokens = result.get_ok();
                return tokens;
            }

        private:
            Result<std::vector<Token::Token>, std::string> result;
            std::string _filename = "bnf.bnf"; 
            bool ignore = false;
            
    };
}

#endif


