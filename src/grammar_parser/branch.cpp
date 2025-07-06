#include "../../include/grammar_parser/branch.h"

void Branch::print(std::ostream& os) const {
    for(const auto& elem : terms){
        os << elem << " ";
    }

    os << " [non_terminals: " << num_pointer_terms() << "] ";
}

void Branch::add(const Term& term){
    if(term.is_rule_pointer()) pointer_terms.push_back(std::make_shared<Term>(term));
    terms.push_back(term);
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

bool Branch::pointer_terms_in(std::vector<U64> term_hashes) const {
    for(const U64& term_hash : term_hashes){

        auto it = std::find_if(pointer_terms.begin(), pointer_terms.end(), [term_hash](const std::shared_ptr<Term>& ptr){
            return ptr && (*ptr == term_hash);
        });

        if(it != pointer_terms.end()){
            return true;
        }
    }

    return false;
}

