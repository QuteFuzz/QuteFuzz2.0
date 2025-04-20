#ifndef RULE_H
#define RULE_H

#include "branch.h"
#include "constraints.h"

class Rule {

    public:
        Rule(){}
        Rule(const std::string& _name) : name(_name) {}
        Rule(const std::vector<Branch>& _branches) : branches(_branches) {}
        ~Rule(){}

        std::string get_name() const {return name;}

        bool get_recursive_flag() const {return recursive;}
        
        void print(std::ostream& os) const;
        
        std::vector<Branch> get_branches(){return branches;}

        void add(const Branch& b);

        size_t size(){return branches.size();}

        bool is_empty() const {return branches.empty();}

        void assign_prob(const float _prob);

        Branch pick_branch(Constraints::Constraints& constraints);

    private:
        std::string name;
        bool recursive = false;
    
        std::vector<Branch> branches;

};

#endif