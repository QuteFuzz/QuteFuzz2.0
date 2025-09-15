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
        RULE_KINDS_TOP,
/*
        RULE_KINDS_TOP: add new rule types below
*/
        RULE,
        H,
        X,
        Y,
        Z,
        RZ,
        RX,
        RY,
        U1,
        S,
        SGD,
        T,
        TDG,
        V,
        VDG,
        PHASEDXPOWGATE,
        PROJECT_Z,
        MEASURE_AND_RESET,
        MEASURE,
        CX,
        CY,
        CZ,
        CCX,
        U2,
        CNOT,
        CH,
        CRZ,
        U3,
        CSWAP,
        TOFFOLI,
        U,
        BARRIER,
        SUBROUTINE_DEFS,
        BLOCK,
        BODY,
        QUBIT_DEFS,
        BIT_DEFS,
        QUBIT_DEF,
        REGISTER_QUBIT_DEF,
        SINGULAR_QUBIT_DEF,
        REGISTER_BIT_DEF,
        SINGULAR_BIT_DEF,
        CIRCUIT_NAME,
        FLOAT_LIST,
        FLOAT_LITERAL,
        MAIN_CIRCUIT_NAME,
        QUBIT_DEF_NAME,
        BIT_DEF_NAME,
        QREG_SIZE,
        CREG_SIZE,
        QUBIT,
        BIT,
        QUBIT_OP,
        GATE_OP,
        SUBROUTINE_OP,
        GATE_MAME,
        QUBIT_LIST,
        BIT_LIST,
        QUBIT_DEF_LIST,
        QUBIT_DEF_SIZE,
        BIT_DEF_LIST,
        BIT_DEF_SIZE,
        SINGULAR_QUBIT,
        REGISTER_QUBIT,
        SINGULAR_BIT,
        REGISTER_BIT,
        QUBIT_NAME,
        BIT_NAME,
        QUBIT_INDEX,
        BIT_INDEX,
        SUBROUTINE,
        CIRCUIT_ID,
        INDENT,
        DEDENT,
        IF_STMT,
        ELSE_STMT,
        ELIF_STMT,
        DISJUNCTION,
        CONJUNCTION,
        INVERSION,
        EXPRESSION,
        COMPARE_OP_BITWISE_OR_PAIR,
        NUMBER,
        SUBROUTINE_OP_ARGS,
        GATE_OP_ARGS,
        SUBROUTINE_OP_ARG,
/*
        RULE_KINDS_BOTTOM: add new rule kinds above
*/
        RULE_KINDS_BOTTOM,

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
            if(t.kind == SYNTAX) std::cout << t.kind << " " << std::quoted(t.value);        
            else std::cout << t.kind << " " << t.value;        
            
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

        // {R"(subroutine_defs)", Token::SUBROUTINE_DEFS},
        // {R"(block)", Token::BLOCK},
        // {R"(body)", Token::BODY},
        // {R"(qubit_defs)", Token::QUBIT_DEFS},
        // {R"(bit_defs)", Token::BIT_DEFS},
        // {R"(qubit_def)", Token::QUBIT_DEF},
        // {R"(register_qubit_def)", Token::REGISTER_QUBIT_DEF},
        // {R"(singular_qubit_def)", Token::SINGULAR_QUBIT_DEF},
        // {R"(register_bit_def)", Token::REGISTER_BIT_DEF},
        // {R"(singular_bit_def)", Token::SINGULAR_BIT_DEF},
        // {R"(circuit_name)", Token::CIRCUIT_NAME},
        // {R"(float_list)", Token::FLOAT_LIST},
        // {R"(float_literal)", Token::FLOAT_LITERAL},
        // {R"(main_circuit_name)", Token::MAIN_CIRCUIT_NAME},
        // {R"(qubit_def_name)", Token::QUBIT_DEF_NAME},
        // {R"(bit_def_name)", Token::BIT_DEF_NAME},
        // {R"(qreg_size)", Token::QREG_SIZE},
        // {R"(creg_size)", Token::CREG_SIZE},
        // {R"(qubit)", Token::QUBIT},
        // {R"(bit)", Token::BIT},
        // {R"(qubit_op)", Token::QUBIT_OP},
        // {R"(gate_op)", Token::GATE_OP},
        // {R"(subroutine_op)", Token::SUBROUTINE_OP},
        // {R"(gate_name)", Token::GATE_MAME},
        // {R"(qubit_list)", Token::QUBIT_LIST},
        // {R"(bit_list)", Token::BIT_LIST},
        // {R"(qubit_def_list)", Token::QUBIT_DEF_LIST},
        // {R"(qubit_def_size)", Token::QUBIT_DEF_SIZE},
        // {R"(bit_def_list)", Token::BIT_DEF_LIST},
        // {R"(bit_def_size)", Token::BIT_DEF_SIZE},
        // {R"(singular_qubit)", Token::SINGULAR_QUBIT},
        // {R"(register_qubit)", Token::REGISTER_QUBIT},
        // {R"(singular_bit)", Token::SINGULAR_BIT},
        // {R"(register_bit)", Token::REGISTER_BIT},
        // {R"(qubit_name)", Token::QUBIT_NAME},
        // {R"(bit_name)", Token::BIT_NAME},
        // {R"(qubit_index)", Token::QUBIT_INDEX},
        // {R"(bit_index)", Token::BIT_INDEX},
        // {R"(subroutine)", Token::SUBROUTINE},
        // {R"(circuit_id)", Token::CIRCUIT_ID},
        // {R"(INDENT)", Token::INDENT},
        // {R"(DEDENT)", Token::DEDENT},
        // {R"(if_stmt)", Token::IF_STMT},
        // {R"(else_stmt)", Token::ELSE_STMT},
        // {R"(elif_stmt)", Token::ELIF_STMT},
        // {R"(disjunction)", Token::DISJUNCTION},
        // {R"(conjunction)", Token::CONJUNCTION},
        // {R"(inversion)", Token::INVERSION},
        // {R"(expression)", Token::EXPRESSION},
        // {R"(compare_op_bitwise_or_pair)", Token::COMPARE_OP_BITWISE_OR_PAIR},
        // {R"(NUMBER)", Token::NUMBER},
        // {R"(subroutine_op_args)", Token::SUBROUTINE_OP_ARGS},
        // {R"(gate_op_args)", Token::GATE_OP_ARGS},
        // {R"(subroutine_op_arg)", Token::SUBROUTINE_OP_ARG},

        // {R"(h)", Token::H},
        // {R"(x)", Token::X},
        // {R"(y)", Token::Y},
        // {R"(z)", Token::Z},
        // {R"(rz)", Token::RZ},
        // {R"(rx)", Token::RX},
        // {R"(ry)", Token::RY},
        // {R"(u1)", Token::U1},
        // {R"(s)", Token::S},
        // {R"(sgd)", Token::SGD},
        // {R"(t)", Token::T},
        // {R"(tdg)", Token::TDG},
        // {R"(v)", Token::V},
        // {R"(vdg)", Token::VDG},
        // {R"(phasedxpowgate)", Token::PHASEDXPOWGATE},
        // {R"(project_z)", Token::PROJECT_Z},
        // {R"(measure_and_reset)", Token::MEASURE_AND_RESET},
        // {R"(measure)", Token::MEASURE},
        // {R"(cx)", Token::CX},
        // {R"(cy)", Token::CY},
        // {R"(cz)", Token::CZ},
        // {R"(ccx)", Token::CCX},
        // {R"(u2)", Token::U2},
        // {R"(cnot)", Token::CNOT},
        // {R"(ch)", Token::CH},
        // {R"(crz)", Token::CRZ},
        // {R"(u3)", Token::U3},
        // {R"(cswap)", Token::CSWAP},
        // {R"(toffoli)", Token::TOFFOLI},
        // {R"(u)", Token::U},
        // {R"(barrier)", Token::BARRIER},

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


