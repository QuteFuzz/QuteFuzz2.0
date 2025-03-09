#include "../include/term.h"

template<>
void Branch::print(std::ostream& os) const {
    for(const auto& elem : coll){
        os << elem << " ";
    }

    os << "[" << _prob << "] rec: " << recursive;
}

template<>
void Rule::print(std::ostream& os) const {
    for(const auto& elem : coll){
        elem.print(os);
        os << "| ";
    }
}

template<>
bool Branch::is_empty() const {
    return coll.empty();
}

template<>
void Branch::assign_prob(const float prob){
    _prob = prob;
}

template<>
void Rule::assign_prob(const float prob){

    for(Branch& b : coll){
        b.assign_prob(prob);
    }
}

/// @brief need to have this check and store pointers to recursive branches separately
/// @param branch 
template<>
void Rule::add(const Branch& branch){
    //if(branch.get_recursive_flag()){
    //    recursive_branches.push_back(branch);
    //}

    coll.push_back(branch);
}