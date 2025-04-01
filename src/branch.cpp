#include "branch.h"

void Branch::print(std::ostream& os) const {
    for(const auto& elem : terms){
        os << elem << " ";
    }

    os << "[" << prob << "]";
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

