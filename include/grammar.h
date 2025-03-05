#ifndef PARSER_H
#define PARSER_H

#include "lex.h"
#include <errno.h>
#include <math.h>
#include <unordered_map>
#include "node.h"

class Grammar{

    public:
        Grammar(){}

        Grammar(const fs::path& filename);

        void consume(int n);

        void consume(const Token_kind kind);

        void peek();

        bool is_rule_end();

        bool is_grammar_end();

        std::shared_ptr<Rule> get_rule_pointer(const std::string& rule_name);

        void assign_equal_probabilities();

        void build_branch(Branch& branch, const Token& token);

        void build_rule(std::shared_ptr<Rule> current_rule);

        void build_grammar();

        void print_grammar();

        inline std::string get_name(){return name;}

        inline std::string get_path(){return path.string();}

    private:
        std::vector<Token> tokens;
        size_t num_tokens;
        size_t token_pointer = 0;
        Result<Token, std::string> curr_token;
        Result<Token, std::string> next_token;

        std::string name;
        fs::path path;

        bool assign_equal_probs = false;

        std::unordered_map<std::string, std::shared_ptr<Rule>> rule_pointers;

        Lexer lexer;
};


#endif
