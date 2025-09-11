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

        std::shared_ptr<Rule> get_rule_pointer(std::string rule_name, U8 scope = NO_SCOPE);

        inline void reset_current_branches(){current_branches.clear();}
        
        inline void add_current_branches_to_rule(){

            if(current_branches.size() == 0){
                current_rule->add(Branch());
                
            } else {

                for(Branch& current_branch : current_branches){
                    #if 0
                    std::cout << "Lazily adding ";
                    current_branch.print(std::cout);
                    std::cout << std::endl;
                    #endif

                    current_rule->add(current_branch);
                }
            }
        }

        /// we just completed a rule, add the current branch to the rule
        /// Called at end of rule, and at each branch seprator
        inline void complete_rule(){
            add_current_branches_to_rule();
        }

        inline void increment_nesting_depth_base(){
            if(nesting_depth == nesting_depth_base) {
                nesting_depth_base += 1;
                nesting_depth = nesting_depth_base;
            }
        }

        inline bool is_wilcard(const Token::Token& token) const {
            return (token.kind ==  Token::OPTIONAL) || (token.kind == Token::ZERO_OR_MORE) || (token.kind == Token::ONE_OR_MORE);
        }

        void extend_current_branches(const Token::Token& wildcard);

        void add_term_to_current_branches(const Token::Token& tokens);

        void add_term_to_branch(const Token::Token& token, Branch& branch);

        void build_grammar();

        void print_grammar() const;

        void print_rules() const;

        void print_tokens() const;

        /// @brief TODO: compare with varying rule scope
        /// @param rule_name 
        /// @param scope 
        /// @return 
        inline bool is_rule(const std::string& rule_name, const U8& scope = NO_SCOPE){
            Rule dummy(rule_name, scope);
            return rule_defined(dummy);            
        }

        inline std::string get_name(){return name;}

        inline std::string get_path(){return path.string();}

        std::vector<std::shared_ptr<Rule>> get_rule_pointers() const {
            return rule_pointers;
        }

        Grammar& operator+=(const Grammar& other){
            std::vector<std::shared_ptr<Rule>> other_rule_pointers = other.get_rule_pointers();

            for(const auto& other_ptr: other_rule_pointers){

                if(!rule_defined(*other_ptr)){
                    rule_pointers.push_back(other_ptr);
                }
            }
        
            return *this;
        }

        bool rule_defined(const Rule& other){
            for(const auto& ptr : rule_pointers){
                if(*ptr == other){return true;}
            }

            return false;
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

        U8 rule_def_scope = NO_SCOPE;
        U8 rule_decl_scope = NO_SCOPE;

        unsigned int nesting_depth_base = 0;
        unsigned int nesting_depth = nesting_depth_base;

        std::vector<std::shared_ptr<Rule>> rule_pointers;
        
        Lexer::Lexer lexer;
        std::string name;
        fs::path path;
};


#endif
