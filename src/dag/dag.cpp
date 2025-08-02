#include <qubit.h>
#include <collection.h>
#include <dag.h>

Dag::Heuristics::Heuristics(const Collection<Qubit::Qubit>& qubits)
{   
    // prepare adj_list
    for(const Qubit::Qubit& qubit : qubits){
        qubit.add_path_to_adj_list(adj_list);
    }

    n_nodes = adj_list.size();

    std::cout << *this;
}

int Dag::Heuristics::score(){
    return 0;
}