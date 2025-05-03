#ifndef BRANCH_H
#define BRANCH_H

#include "term.h"

typedef struct {
    std::vector<Term> remainders;
    std::vector<Term> mults;

    bool is_empty(){
        return remainders.empty() && mults.empty();
    }

    void clear(){
        remainders.clear();
        mults.clear();
    }

} Branch_multiply;

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

        void assign_prob(const float _prob){prob = _prob;}

        float get_prob() const {return prob;}

        void add(const Term& term);

        size_t size() const {return terms.size();}

        inline size_t num_pointer_terms() const {return pointer_terms.size();}

        bool pointer_terms_match(std::vector<U64> term_hashes) const ;

        bool pointer_terms_in(std::vector<U64> term_hashes) const ; 

        bool is_empty() const {return terms.empty();}

        std::vector<Term> get_terms(){return terms;} 

        void setup_basis(Branch_multiply& basis, unsigned int nesting_depth) const;

        void print(std::ostream& os) const;

    private:
        std::string name;
        bool recursive = false;

        std::vector<Term> terms;
        std::vector<std::shared_ptr<Term>> pointer_terms;
        float prob = 0.0;
};


#endif
