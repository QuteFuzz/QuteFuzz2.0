#include "../include/rule.h"

void Rule::print(std::ostream& os) const {
    for(const auto& elem : branches){
        elem.print(os);
        os << "| ";
    }
}

void Rule::assign_prob(const float _prob){
    for(Branch& b : branches){
        b.assign_prob(_prob);
    }
}


/// @brief need to have this check and store pointers to recursive branches separately
/// @param branch 
void Rule::add(const Branch& branch){
    branches.push_back(branch);

    if(branch.get_recursive_flag()){
        recursive = true; // this rule is recursive
    }
}

/// @brief Randomly pick a branch from the rule that satisfies a given constraint
/// @param c 
/// @return 
Branch Rule::pick_branch(Constraints::Constraints& constraints){
    size_t size = branches.size();

    if(size > 0){
        int rand_index = random_int(size - 1);
        Branch branch = branches[rand_index];
        U64 hashed_rule_name = hash_rule_name(name);

        while(!constraints.are_satisfied(hashed_rule_name, branch)){
            // std::cout << "Branch " << rand_index << " for " << name << std::endl;

            rand_index = random_int(size - 1);
            branch = branches[rand_index];
        }

        return branch;

    } else {
        throw std::runtime_error("There are no branches for rule " + name);
    }
}

