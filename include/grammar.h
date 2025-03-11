#ifndef PARSER_H
#define PARSER_H

#include "lex.h"
#include <errno.h>
#include <math.h>
#include <unordered_map>
#include "term.h"
#include <cctype>
#include <algorithm>

using Expansion_option = std::vector<Token>;
using Expansions = std::vector<Expansion_option>;

class Grammar{

    public:
        Grammar(){}

        Grammar(const fs::path& filename);

        void consume(int n);

        void consume(const Token_kind kind);

        void peek();

        bool is_alpha(const std::string& str);

        std::shared_ptr<Rule> get_rule_pointer(const std::string& rule_name);

        void assign_equal_probabilities();

        bool in_variant_grouping(const Token& current_token);

        void expand_range(const Token& from, const Token& to);

        void add_n_branches(const Token& next);

        void reset_current_branches(){
            current_branches = {Branch()};
        }
        
        void lazily_add_branches_to_rule(){
            // add all current branches to current rule, reset current branches
            for(Branch& current_branch : current_branches){
                // std::cout << "Lazily adding ";
                // current_branch.print(std::cout);
                // std::cout << std::endl;

                current_rule->add(current_branch);   
            }
        }

        /// we just completed a rule, add the current branch to the rule, and assign probabilities for branches of this rule
        void complete_rule(){
            lazily_add_branches_to_rule();
            assign_equal_probabilities();
        }

        void drop_heads(){
            // std::cout << expanded_branches_head << std::endl;
            current_branches = std::vector<Branch>(current_branches.begin() + expanded_branches_head, current_branches.end());
        }

        void add_term_to_current_branches(const Token& tokens);

        void add_term_to_branch(const Token& token, Branch& branch);

        void build_grammar();

        void print_grammar() const;

        void print_rules() const;

        inline std::string get_name(){return name;}

        inline std::string get_path(){return path.string();}

    private:
        std::vector<Token> tokens;
        size_t num_tokens = 0;
        size_t token_pointer = 0;
        Result<Token, std::string> curr_token;
        Result<Token, std::string> next_token;
        Token prev_token;

        std::vector<Branch> current_branches = {Branch()}; // when performing expansions, there may be more than one branch to continue building ((expr)+, (expr)*)
        std::shared_ptr<Rule> current_rule = nullptr;
        size_t expanded_branches_head = 0;

        // * ? + expansion stores
        unsigned int in_grouping = 0;
        Expansions expansion_tokens = {{}};
        int wildcard_max = WILDCARD_MAX;
        bool just_finished_grouping = false;

        bool assign_equal_probs = false;

        std::unordered_map<std::string, std::shared_ptr<Rule>> rule_pointers;

        Lexer lexer;
        std::string name;
        fs::path path;
};


#endif
