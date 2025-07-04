#ifndef RULE_H
#define RULE_H

#include "branch.h"
#include "../ast_builder/constraints.h"

class Rule {

    public:
        Rule(){}
        Rule(const std::string& _name) : name(_name) {
            hash = hash_rule_name(name);
        }
        Rule(const std::vector<Branch>& _branches) : branches(_branches) {}
        ~Rule(){}

        std::string get_name() const {return name;}

        U64 get_hash() const {return hash;}

        bool get_recursive_flag() const {return recursive;}
        
        void print(std::ostream& os) const;
        
        std::vector<Branch> get_branches(){return branches;}

        void add(const Branch& b);

        size_t size(){return branches.size();}

        bool is_empty() const {return branches.empty();}

        void assign_prob(const float _prob);

        Branch pick_branch(Constraints::Constraints& constraints);

        inline void mark_as_common(){common = true;} 

        bool is_marked_as_common(){return common;}

    private:
        std::string name;
        U64 hash = 0ULL;
        bool recursive = false; //Toggles recursion
        bool common = false;
    
        std::vector<Branch> branches;

};

#endif