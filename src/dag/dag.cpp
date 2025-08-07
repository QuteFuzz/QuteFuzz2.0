#include <resource.h>
#include <collection.h>
#include <dag.h>

Dag::Heuristics::Heuristics(const Collection<Resource::Qubit>& qubits) {
    // prepare adj_list
    for(const Resource::Qubit& qubit : qubits){
        qubit.add_path_to_heuristics(*this);
    }

    n_nodes = data.size();

    std::cout << *this;
}

/// @brief Combine heuristics to get dag score
/// @return 
int Dag::Heuristics::score(){   
    return max_out_degree();
}