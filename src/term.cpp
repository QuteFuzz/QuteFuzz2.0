#include "../include/term.h"

void Branch::print(std::ostream& os) const {
    for(const auto& elem : terms){
        os << elem << " ";
    }

    os << "[" << prob << "]";
}

void Rule::print(std::ostream& os) const {
    for(const auto& elem : branches){
        elem.print(os);
        os << "| ";
    }

    std::cout << " non_recursive branches: " << non_recursive_branches.size() << std::endl;
}

void Rule::assign_prob(const float _prob){
    for(Branch& b : branches){
        b.assign_prob(_prob);
    }
}

void Branch::add(const Term& term){
    terms.push_back(term);
}

/// @brief need to have this check and store pointers to recursive branches separately
/// @param branch 
void Rule::add(const Branch& branch){
    branches.push_back(branch);

    if(branch.get_recursive_flag()){
        recursive = true; // this rule is recursive
    } else {
        std::shared_ptr<Branch> ptr = std::make_shared<Branch>(branch);
        non_recursive_branches.push_back(ptr); // this branch is non-recursive
    }
}

Branch Rule::pick_non_recursive_branch(){
    size_t size = non_recursive_branches.size();

    if(size > 0){
        int rand_index = random_int(size - 1);
        return *(non_recursive_branches[rand_index]);
    } else {
        throw std::runtime_error("There are no non-recursive branches for rule " + _name);
    }
}

/// @brief Never called on an empty branch, multiplier always starts at 2
/// @param multiplier 
/// @param only_last 
/// @return 
Branch Branch::multiply_terms(unsigned int multiplier, unsigned int nesting_depth) const {
    std::vector<Term> mult_terms; 

    for(unsigned int i = 1; i <= nesting_depth; ++i){
        for(const Term& t : terms){
            mult_terms.push_back(t);
        }
    }

    return Branch(mult_terms);
}

