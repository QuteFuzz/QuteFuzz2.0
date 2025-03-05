#ifndef TERM_H
#define TERM_H

#include "utils.h"

typedef enum {
    TK_SYNTAX,
    TK_POINTER
} Term_kind;

class Term;
template<typename T> class Collection;

using Branch = Collection<Term>;
using Rule = Collection<Branch>;

class Term {
    public:
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

template<typename T>
class Collection{

    public:
        Collection(){}
        Collection(const std::string& name) :_name(name){}
        ~Collection(){}

        std::string get_name(){return _name;}

        void add(const T& elem){
            coll.push_back(elem);
        }

        size_t size(){
            return coll.size();
        }

        void set_recursive_flag(std::shared_ptr<Rule> parent_rule);

        bool get_recursive_flag() const {return recursive;}

        void assign_prob(const float prob);

        float get_prob() const {return _prob;}

        std::vector<T> get_coll() const {return coll;}

        bool is_empty() const;

        void print(std::ostream& os) const;

    private:
        std::vector<T> coll;
        std::string _name;
        float _prob = 0.0;
        bool recursive = false;

};


#endif

