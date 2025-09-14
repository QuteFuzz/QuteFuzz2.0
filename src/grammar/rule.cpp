#include <rule.h>
#include <node.h>

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
        INFO("Picking branch for " + name + STR_SCOPE(scope) + " while satisfying constraint " + parent->get_debug_constraint_string());
        #endif

        while(!parent->branch_satisfies_constraint(branch)){
            branch = branches[random_int(size - 1)];
        }

        return branch;

    } else {
        #ifdef DEBUG
        INFO(name + STR_SCOPE(scope) + " is an empty rule");
        #endif

        return Branch();
    }
}

