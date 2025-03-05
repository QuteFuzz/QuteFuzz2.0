#include "../include/node.h"

template<>
void Branch::print(std::ostream& os) const {
    for(const auto& elem : coll){
        os << elem << " ";
    }

    os << "[" << _prob << "]";
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

template<>
void Rule::set_recursive_flag(std::shared_ptr<Rule> current_rule){
    recursive = true;
}

template<>
void Branch::set_recursive_flag(std::shared_ptr<Rule> parent_rule){
    recursive = false;

    for(const Node& node : coll){
        if (node.name_matches(parent_rule->get_name())){
            recursive = true;
            parent_rule->set_recursive_flag(nullptr);
            break;
        }
    }

    // std::cout << "Branch " << _name << "flag " << recursive << std::endl; 
}
