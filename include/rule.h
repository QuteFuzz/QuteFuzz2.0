#ifndef RULE_H
#define RULE_H

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

        size_t num_rules() const;

        bool is_empty() const {return terms.empty();}

        std::vector<Term> get_terms(){return terms;} 

        void setup_basis(Branch_multiply& basis, unsigned int nesting_depth) const;

        void print(std::ostream& os) const;

    private:
        std::string name;
        bool recursive = false;

        std::vector<Term> terms;
        float prob = 0.0;
};

/// @brief A constraint on some property of a branch
namespace Constraints {
    typedef enum {
        MINIMUM,
        MAXIMUM,
        EQUALS,
        NON_RECURSIVE,
    } Type;

    typedef struct {
        Type type;
        unsigned int value = 0;
        uint64_t node = 0; // constraint on a branch from a particular node

        bool is_satisfied(const uint64_t _node, const Branch& b) const {

            switch(type){
                case MAXIMUM: return (b.num_rules() <= value) || (node != _node);
                case MINIMUM: return (b.num_rules() >= value) || (node != _node);
                case EQUALS: return (b.num_rules() == value) || (node != _node);
                case NON_RECURSIVE: return !b.get_recursive_flag();
            }

            return false;
        }

    } Constraint;

    struct Constraints {
        
        public:
            Constraints(){}

            /// @brief Check that all constraints on this branch are satisfied
            /// @param b 
            /// @return 
            bool are_satisfied(const uint64_t _node, const Branch& b) const {
                
                for(const Constraint& c : constraints){
                    if(!c.is_satisfied(_node, b)) return false;
                }
                return true;
            }

            void add_constraint(Constraint c){
                constraints.push_back(c);
            }

            void clear(){
                constraints.clear();
            }
        
        private:
            std::vector<Constraint> constraints = {};

    };
}

class Rule {

    public:
        Rule(){}
        Rule(const std::string& _name) : name(_name), gen(rd()) {}
        ~Rule(){}

        std::string get_name() const {return name;}

        bool get_recursive_flag() const {return recursive;}
        
        void print(std::ostream& os) const;
        
        std::vector<Branch> get_branches(){return branches;}

        void add(const Branch& b);

        size_t size(){return branches.size();}

        bool is_empty() const {return branches.empty();}

        void assign_prob(const float _prob);

        inline int random_int(int maximum_index){
            std::uniform_int_distribution<int> int_dist(0, maximum_index);
            return int_dist(gen);
        }

        Branch pick_branch(Constraints::Constraints& constraints);

    private:
        std::string name;
        bool recursive = false;
    
        std::vector<Branch> branches;
        std::random_device rd;
        std::mt19937 gen;
};

#endif