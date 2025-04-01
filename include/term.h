#ifndef TERM_H
#define TERM_H

#include "utils.h"

class Rule;

class Term {
    public:
        Term(){}
        Term(const std::string& name, unsigned int nd): _name(name), nesting_depth(nd) {}
        ~Term() = default;

        void set(std::shared_ptr<Rule> term){
            value = term;
            hashed_name = hash_rule_name(_name);
        }

        void set(std::string syntax){
            value = syntax;
            hashed_name = hash_rule_name(syntax);
        }

        std::shared_ptr<Rule> get_rule() const {
            if(is_pointer()){return std::get<std::shared_ptr<Rule>>(value);}
            else {
                throw std::runtime_error("get_term called on syntax!");
            }
        }

        std::string get_syntax() const {
            if(is_syntax()){return std::get<std::string>(value);}
            else {
                throw std::runtime_error("get_syntax called on pointer!");
            }
        }

        std::string get_string() const {
            return _name;
        }

        unsigned int get_value() const {
            return hashed_name;
        }

        bool is_syntax() const {
            return std::holds_alternative<std::string>(value);
        }

        bool is_pointer() const {
            return std::holds_alternative<std::shared_ptr<Rule>>(value);
        }

        friend std::ostream& operator<<(std::ostream& stream, Term term){
            if(term.is_syntax()){
                stream << "\"" << term.get_syntax() << "\"";
            } else {
                stream << term._name;
            }

            return stream;
        }

        void set_nesting_depth(unsigned int nd){nesting_depth = nd;}

        unsigned int get_nesting_depth() const {return nesting_depth;}

        /// @brief Combine syntax terms. This is unsafe, but will throw errors if `get_syntax` is called on pointer
        /// @param ext 
        Term& operator+=(const Term& ext){
            auto s = get_syntax() + ext.get_syntax();
            set(s);

            return *this;
        }

    private:
        std::variant<std::shared_ptr<Rule>, std::string> value;

        std::string _name;
        unsigned int hashed_name = 0;
        unsigned int nesting_depth = 0;
};

#endif

