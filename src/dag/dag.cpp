#include <qubit.h>
#include <bit.h>
#include <collection.h>
#include <dag.h>

Dag::Heuristics::Heuristics(const Collection<Qubit::Qubit>& qubits, const Collection<Bit::Bit>& bits) {
    // prepare adj_list
    for(const Qubit::Qubit& qubit : qubits){
        qubit.add_path_to_heuristics(*this);
    }

    for(const Bit::Bit& bit : bits){
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