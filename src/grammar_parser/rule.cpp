#include <rule.h>
#include <node.h>

void Rule::print(std::ostream& os) const {
    for(const auto& elem : branches){
        elem.print(os);
        os << "| ";
    }

    os << " common: " << common;
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

Branch Rule::pick_branch(std::shared_ptr<Node> parent){
    size_t size = branches.size();

    if(size > 0){
        Branch branch = branches[random_int(size - 1)];

        #ifdef DEBUG
        INFO("Picking branch for " + name + " while satisfying constraint");
        #endif

        while(!parent->branch_satisfies_constraint(branch)){
            branch = branches[random_int(size - 1)];
        }

        return branch;

    } else {
        return Branch();
    }
}

