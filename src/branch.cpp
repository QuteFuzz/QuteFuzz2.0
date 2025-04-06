#include "../include/branch.h"

void Branch::print(std::ostream& os) const {
    for(const auto& elem : terms){
        os << elem << " ";
    }

    os << " [non_terminals: " << num_pointer_terms() << "] ";
}

/// @brief When adding a term to a branch, check whether it can be combined with the previous term that was added
/// @param term 
void Branch::add(const Term& term){
    size_t size = terms.size();

    if(term.is_pointer() && !Common::is_common(term.get_string())) pointer_terms.push_back(std::make_shared<Term>(term));

    if(size){
        auto last_term = terms.end()-1; // reference to the last term in the branch
        
        if(last_term->is_syntax() && term.is_syntax()) *last_term += term;
        else terms.push_back(term);

    } else {
        terms.push_back(term);

    }
}

/// @brief Given the current branch and the nesting depth, work out which terms within a branch should be multiplied. The term's grouping scope tells you that, which is 
/// infered by comparing with the nesting depth. Terms with a nesting depth strictly greater than it are multiplied, the rest are left alone. This split is stored inside the 
/// `Branch_multiply` struct.
/// @param basis 
/// @param nesting_depth 
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

bool Branch::pointer_terms_match(std::vector<U64> term_hashes) const {
    size_t size = num_pointer_terms();

    if(size == term_hashes.size()){
        
        for(size_t i = 0; i < size; ++i){
            Term t = *pointer_terms[i];
            if (!(t == term_hashes[i])) return false;
        }

        return true;

    } else {
        return false;
    }
}