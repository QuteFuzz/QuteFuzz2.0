#ifndef TERM_H
#define TERM_H

#include "utils.h"

typedef enum {
    TK_SYNTAX,
    TK_POINTER
} Term_kind;

class Rule;

class Term {
    public:
        Term(){}
        Term(const std::string& name) :_name(name){}
        ~Term() = default;

        void set_pointer(std::shared_ptr<Rule> term){
            value = term;
            kind = TK_POINTER;
        }

        void set_syntax(std::string syntax){
            value = syntax;
            kind = TK_SYNTAX;
        }

        std::shared_ptr<Rule> get_rule() const {
            if(kind == TK_POINTER){return std::get<std::shared_ptr<Rule>>(value);}
            else {
                throw std::runtime_error("get_term called on syntax!");
            }
        }

        std::string get_syntax() const {
            if(kind == TK_SYNTAX){return std::get<std::string>(value);}
            else {
                throw std::runtime_error("get_syntax called on term!");
            }
        }

        bool name_matches(const std::string& name) const {
            return _name == name;
        }

        bool is_syntax() const {
            return is(TK_SYNTAX);
        }

        bool is_pointer() const {
            return is(TK_POINTER);
        }

        friend std::ostream& operator<<(std::ostream& stream, Term term){
            if(term.kind == TK_SYNTAX){
                stream << "\"" << term.get_syntax() << "\"";
            } else {
                stream << term._name;
            }

            return stream;
        }

    protected:
        bool is(Term_kind nk) const {
            return kind == nk;
        }

        Term_kind kind;
        std::variant<std::shared_ptr<Rule>, std::string> value;
        std::string _name;
};

class Collection{

    public:
        Collection(){}
        Collection(const std::string& name) :_name(name) {}
        ~Collection(){}

        std::string get_name(){return _name;}

        bool get_recursive_flag() const {return recursive;}

        virtual size_t size() = 0;

        virtual bool is_empty() = 0;

        virtual void print(std::ostream& os) const = 0;

    protected:
        std::string _name;
        bool recursive = false;
};

class Branch : public Collection {

    public:
        using Collection::Collection;

        inline void set_recursive_flag(){recursive = true;}

        void assign_prob(const float _prob){prob = _prob;}

        float get_prob() const {return prob;}

        void add(const Term& term);

        size_t size(){return terms.size();}

        bool is_empty(){return terms.empty();}

        std::vector<Term> get_terms(){return terms;} 

        void print(std::ostream& os) const;

    private:
        std::vector<Term> terms;
        float prob;
};

class Rule : public Collection {

    public:
        Rule(){}
        Rule(const std::string& _name) : Collection(_name), gen(rd()) {}
        ~Rule(){}
        
        void print(std::ostream& os) const;
        
        std::vector<Branch> get_branches(){return branches;}

        void add(const Branch& b);

        size_t size(){return branches.size();}

        bool is_empty(){return branches.empty();}

        void assign_prob(const float _prob);

        inline int random_int(int maximum_index){
            std::uniform_int_distribution<int> int_dist(0, maximum_index);
            return int_dist(gen);
        }

        Branch pick_non_recursive_branch();

    private:
        std::vector<Branch> branches;
        std::vector<std::shared_ptr<Branch>> non_recursive_branches;
        std::random_device rd;
        std::mt19937 gen;
};

#endif

