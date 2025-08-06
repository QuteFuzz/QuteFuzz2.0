#include <resource.h>
#include <collection.h>
#include <dag.h>

Dag::Heuristics::Heuristics(const Collection<Resource::Resource>& qubits, const Collection<Resource::Resource>& bits) {
    // prepare adj_list
    for(const Resource::Resource& qubit : qubits){
        qubit.add_path_to_heuristics(*this);
    }

    for(const Resource::Resource& bit : bits){
        bit.add_path_to_heuristics(*this);
    }

    n_nodes = data.size();

    std::cout << *this;
}

/// @brief Combine heuristics to get dag score
/// @return 
int Dag::Heuristics::score(){   
    return max_out_degree();
}