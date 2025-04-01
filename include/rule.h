#ifndef RULE_H
#define RULE_H

#include "branch.h"

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
        std::vector<std::shared_ptr<Branch>> non_recursive_branches;
        std::random_device rd;
        std::mt19937 gen;
};

#endif