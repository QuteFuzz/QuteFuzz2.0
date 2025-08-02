#include <qubit.h>
#include <collection.h>
#include <dag.h>

Dag::Process::Process(const Collection<Qubit::Qubit>& qubits)
{   
    // prepare adj_list
    for(const Qubit::Qubit& qubit : qubits){
        qubit.add_path_to_adj_list(adj_list);
    }

    std::cout << *this;

}

int Dag::Process::score(){
    return 0;
}