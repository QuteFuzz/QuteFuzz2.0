#include "../include/rule.h"

void Branch::print(std::ostream& os) const {
    for(const auto& elem : terms){
        os << elem << " ";
    }

    os << " [non_terminals: " << num_rules() << "] ";
}

/// @brief When adding a term to a branch, check whether it can be combined with the previous term that was added
/// @param term 
void Branch::add(const Term& term){
    size_t size = terms.size();

    if(size){
        auto last_term = terms.end()-1; // reference to the last term in the branch
        
        if(last_term->is_syntax() && term.is_syntax()) *last_term += term;
        else terms.push_back(term);

    } else {
        terms.push_back(term);

    }
}

/// @brief Never called on an empty branch, multiplier always starts at 2
/// @param multiplier 
/// @param only_last 
/// @return 
void Branch::setup_basis(Branch_multiply& basis, unsigned int nesting_depth) const {

    for(const Term& t : terms){
        if(t.get_nesting_depth() > nesting_depth){  // only multiply the term if it is in the correct grouping scope
            basis.mults.push_back(t);
        } else {
            basis.remainders.push_back(t);
        }
    }

    std::cout << basis.mults.size() << " " << basis.remainders.size() << std::endl;
}

size_t Branch::num_rules() const{
    size_t out = 0;

    for(const Term& t : terms){
        // if the term is a pointer that points to some branch, then it is a non-terminal
        out += (t.is_pointer() && !Common::is_common(t.get_string()));
    }

    return out;
}

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
        uint64_t hashed_rule_name = hash_rule_name(name);

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

