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

        bool in_variant_grouping(const Token& current_token, const Token& next_token);

        bool just_finished_paren_grouping(){return prev_token.kind == TOKEN_RPAREN;}

        bool just_finished_brack_grouping(){return prev_token.kind == TOKEN_RBRACK;}

        bool can_create_branches(){
            bool finished_grouping = just_finished_paren_grouping() || just_finished_brack_grouping();

            return finished_grouping;
        }

        void expand_range();

        void add_n_branches(const Token& next);

        inline void reset_current_branches(){current_branches.clear();}
        
        inline void add_current_branches_to_rule(){
            // add all current branches to current rule, reset current branches
            for(Branch& current_branch : current_branches){
                std::cout << "Lazily adding ";
                current_branch.print(std::cout);
                std::cout << std::endl;

                current_rule->add(current_branch);   
            }
        }

        /// we just completed a rule, add the current branch to the rule, and assign probabilities for branches of this rule
        /// Called at end of rule, and at each branch seprator
        inline void complete_rule(){
            add_current_branches_to_rule();
            assign_equal_probabilities();
        }

        inline void drop_heads(){
            // std::cout << expanded_branches_head << std::endl;
            current_branches = std::vector<Branch>(current_branches.begin() + current_branches_head, current_branches.end());
        }

        void extend_current_branches();

        inline void add_empty_to_current_branches(){current_branches.push_back(Branch());}

        void add_term_to_current_branches(const Token& tokens);

        void add_term_to_branch(const Token& token, Branch& branch);

        void build_grammar();

        void print_grammar() const;

        void print_rules() const;

        void print_tokens() const;

        void add_to_expansion_tokens(const Token& token);

        inline std::string get_name(){return name;}

        inline std::string get_path(){return path.string();}

    private:
        std::vector<Token> tokens;
        size_t num_tokens = 0;
        size_t token_pointer = 0;
        Result<Token, std::string> curr_token;
        Result<Token, std::string> next_token;
        Token prev_token;

        std::string range_start = "", range_end = "";

        std::vector<Branch> current_branches;
        std::shared_ptr<Rule> current_rule = nullptr;
        size_t current_branches_head = 0;

        unsigned int nesting_depth = 0;
        Expansions expansion_tokens;

        bool assign_equal_probs = false;

        std::unordered_map<std::string, std::shared_ptr<Rule>> rule_pointers;

        Lexer lexer;
        std::string name;
        fs::path path;
};


#endif
