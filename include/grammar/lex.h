#ifndef LEX_H
#define LEX_H

#include <regex>
#include <stdio.h>
#include "string.h"
#include <stdlib.h>
#include <fstream>

#include <result.h>

namespace Token {
    enum Token_kind {
        _EOF = 0,
        RULE,
        SEPARATOR,
        RULE_START,
        RULE_END,
        SYNTAX,
        LPAREN,
        LBRACK,
        LBRACE,
        RPAREN,
        RBRACK,
        RBRACE,
        ZERO_OR_MORE,
        ONE_OR_MORE,
        OPTIONAL,
        ARROW,
        INTERNAL,
        EXTERNAL,
        OWNED,
        COMMENT,
        MULTI_COMMENT_START,
        MULTI_COMMENT_END
    };

    struct Token{
        std::string value;
        Token_kind kind;

        friend std::ostream& operator<<(std::ostream& stream, const Token t){
            stream << t.kind << " " << std::quoted(t.value);        
            return stream;
        }
    };
}

namespace Lexer {

    struct Token_rule {
        std::string pattern;
        Token::Token_kind kind;

        std::optional<std::string> value = std::nullopt;
    };

    const std::vector<Token_rule> TOKEN_RULES = {
        {R"(LPAREN)", Token::SYNTAX, std::make_optional<std::string>("(")},
        {R"(RPAREN)", Token::SYNTAX, std::make_optional<std::string>(")")},
        {R"(LBRACK)", Token::SYNTAX, std::make_optional<std::string>("[")},
        {R"(RBRACK)", Token::SYNTAX, std::make_optional<std::string>("]")},
        {R"(LBRACE)", Token::SYNTAX, std::make_optional<std::string>("{")},
        {R"(RBRACE)", Token::SYNTAX, std::make_optional<std::string>("}")},

        {R"(COMMA)", Token::SYNTAX, std::make_optional<std::string>(",")},
        {R"(SPACE)", Token::SYNTAX, std::make_optional<std::string>(" ")},
        {R"(DOT)", Token::SYNTAX, std::make_optional<std::string>(".")},
        {R"(SINGLE_QUOTE)", Token::SYNTAX, std::make_optional<std::string>("\'")},
        {R"(DOUBLE_QUOTE)", Token::SYNTAX, std::make_optional<std::string>("\"")},

        {R"(EQUALS)", Token::SYNTAX, std::make_optional<std::string>("=")},
        {R"(NEWLINE)", Token::SYNTAX, std::make_optional<std::string>("\n")},

        {R"(\(\*)", Token::MULTI_COMMENT_START},
        {R"(\*\))", Token::MULTI_COMMENT_END},
        {R"(EXTERNAL(::)?)", Token::EXTERNAL},
        {R"(INTERNAL(::)?)", Token::INTERNAL},
        {R"(OWNED(::)?)", Token::OWNED},
        {R"([a-zA-Z_]+)", Token::RULE},
        {R"(\".*?\"|\'.*?\')", Token::SYNTAX},
        {R"(=|:)", Token::RULE_START},
        {R"(\|)", Token::SEPARATOR},
        {R"(;)", Token::RULE_END},
        {R"(\()", Token::LPAREN},
        {R"(\))", Token::RPAREN},
        {R"(\[)", Token::LBRACK},
        {R"(\])", Token::RBRACK},
        {R"(\{)", Token::LBRACE},
        {R"(\})", Token::RBRACE},
        {R"(\*)", Token::ZERO_OR_MORE},
        {R"(\?)", Token::OPTIONAL},
        {R"(\+)", Token::ONE_OR_MORE},
        {R"(\-\>)", Token::ARROW},
        {R"(#)", Token::COMMENT}
    };

    const std::string FULL_REGEX = [] {
        std::string regex = "(";

        for (size_t i = 0; i < TOKEN_RULES.size(); i++) {
            regex += TOKEN_RULES[i].pattern;

            if (i + 1 < TOKEN_RULES.size()) regex += "|";
        }
        regex += ")";

        return regex;
    }();
            
    class Lexer{
        public:
            Lexer(){}

            Lexer(std::string filename)
                :_filename(filename)
            {
                lex();
            }

            std::string remove_outer_quotes(const std::string& token){
                if ((token.size() > 2) && 
                    (((token.front() == '\"') && (token.back() == '\"')) ||
                    ((token.front() == '\'') && (token.back() == '\'')))
                ){
                    return token.substr(1, token.size() - 2);
                }
                
                return token;      
            }

            inline bool string_is(const std::string& string, const std::string& pattern){
                bool matches = std::regex_match(string, std::regex(pattern));
                return ((ignore == false) && matches) || (string == "*)") ;
            }

            void lex();

            void print_tokens() const;

            std::vector<Token::Token> get_tokens(){
                std::vector<Token::Token> tokens = result.get_ok();
                return tokens;
            }

        private:
            Result<std::vector<Token::Token>> result;
            std::string _filename = "bnf.bnf"; 
            bool ignore = false;
            
    };
}

#endif


