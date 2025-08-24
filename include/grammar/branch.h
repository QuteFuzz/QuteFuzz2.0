#ifndef BRANCH_H
#define BRANCH_H

#include <term.h>

struct Branch_multiply {
    std::vector<Term> remainders;
    std::vector<Term> mults;

    bool is_empty(){
        return remainders.empty() && mults.empty();
    }

    void clear(){
        remainders.clear();
        mults.clear();
    }

};

class Branch {

    public:
        Branch(){}
        Branch(const std::string& _name) : name(_name){}
        ~Branch(){}

        Branch(std::vector<Term> _terms) {
            // collapse terms
            for(Term& t : _terms){
                add(t);
            }
        }

        std::string get_name() const {return name;}

        bool get_recursive_flag() const {return recursive;}

        inline void set_recursive_flag(){recursive = true;}

        void add(const Term& term);

        size_t size() const {return terms.size();}

        Term at(size_t index) const {return terms.at(index);}

        size_t count_rule_occurances(const U64& hash) const {

            size_t out = 0;

            for(size_t i = 0; i < terms.size(); i++){
                out += (terms[i].is_pointer() && (terms[i] == hash));
            }

            return out;
        }

        bool is_empty() const {return terms.empty();}

        std::vector<Term> get_terms(){return terms;} 

        void setup_basis(Branch_multiply& basis, unsigned int nesting_depth) const;

        void print(std::ostream& os) const;

    private:
        std::string name;
        bool recursive = false;

        std::vector<Term> terms;
};


#endif
