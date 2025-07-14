#ifndef PARSER_H
#define PARSER_H

#include <lex.h>
#include <errno.h>
#include <math.h>
#include <unordered_map>
#include <rule.h>
#include <cctype>
#include <algorithm>

class Grammar{

    public:
        Grammar(){}

        Grammar(const fs::path& filename);

        void consume(int n);

        void consume(const Token::Token_kind kind);

        void peek();

        std::shared_ptr<Rule> get_rule_pointer(std::string rule_name);

        void assign_equal_probabilities();

        inline void reset_current_branches(){current_branches.clear();}
        
        inline void add_current_branches_to_rule(){
            // add all current branches to current rule, reset current branches
            for(Branch& current_branch : current_branches){
                #if 0
                std::cout << "Lazily adding ";
                current_branch.print(std::cout);
                std::cout << std::endl;
                #endif

                current_rule->add(current_branch);
            }
        }

        /// we just completed a rule, add the current branch to the rule, and assign probabilities for branches of this rule
        /// Called at end of rule, and at each branch seprator
        inline void complete_rule(){
            add_current_branches_to_rule();
            assign_equal_probabilities();
        }

        inline void increment_nesting_depth_base(){
            if(nesting_depth == nesting_depth_base) {
                nesting_depth_base += 1;
                nesting_depth = nesting_depth_base;
            }
        }

        bool is_wilcard(const Token::Token& token) const {
            return (token.kind ==  Token::OPTIONAL) || (token.kind == Token::ZERO_OR_MORE) || (token.kind == Token::ONE_OR_MORE);
        }

        void extend_current_branches(const Token::Token& wildcard);

        void add_term_to_current_branches(const Token::Token& tokens);

        void add_term_to_branch(const Token::Token& token, Branch& branch);

        void expand_range();

        void build_grammar();

        void print_grammar() const;

        void print_rules() const;

        void print_tokens() const;

        inline bool is_rule(const std::string rule_name){
            return rule_pointers.find(rule_name) != rule_pointers.end();
        }

        inline std::string get_name(){return name;}

        inline std::string get_path(){return path.string();}

        inline void mark_as_commons_grammar(){
            for(auto& [f, s] : rule_pointers){
                s->mark_as_common();
            }
        }

        std::unordered_map<std::string, std::shared_ptr<Rule>> get_rule_pointers() const {
            return rule_pointers;
        }

        Grammar& operator+=(const Grammar& other){
            std::unordered_map<std::string, std::shared_ptr<Rule>> other_rule_pointers = other.get_rule_pointers();

            for(const auto& [k ,v]: other_rule_pointers){
                rule_pointers.insert_or_assign(k, v);
            }
        
            return *this;
        }

    private:
        std::vector<Token::Token> tokens;
        size_t num_tokens = 0;
        size_t token_pointer = 0;
        Result<Token::Token> curr_token;
        Result<Token::Token> next_token;
        Token::Token prev_token;

        std::string range_start = "", range_end = "";

        std::vector<Branch> current_branches;
        std::shared_ptr<Rule> current_rule = nullptr;

        unsigned int nesting_depth_base = 0;
        unsigned int nesting_depth = nesting_depth_base;

        bool assign_equal_probs = false;

        std::unordered_map<std::string, std::shared_ptr<Rule>> rule_pointers;
        
        Lexer::Lexer lexer;
        std::string name;
        fs::path path;
};


#endif
