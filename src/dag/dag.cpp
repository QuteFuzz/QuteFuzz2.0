#include <resource.h>
#include <collection.h>
#include <dag.h>

void Dag::Dag::make_dag(const Collection<Resource::Qubit>& _qubits){
    data.clear();

    qubits = _qubits;
    
    for(const Resource::Qubit& qubit : qubits){
        qubit.add_path_to_dag(*this);
    }

    n_nodes = data.size();
}

void Dag::Dag::make_dag(const Dag& dag1, const Dag& dag2){
    data.clear();

    qubits = dag1.get_qubits();
    UNUSED(dag2);
}

void Dag::Dag::add_edge(int source_node_id, int dest_node_id){
    if(data.contains(source_node_id)){
        data.at(source_node_id).children.push_back(dest_node_id);
        data.at(source_node_id).out_degree += 1;

    } else {
        data[source_node_id] = {.in_degree = 0, .out_degree = 1, .children = {dest_node_id}}; 
    }

    if(data.contains(dest_node_id)){
        data.at(dest_node_id).in_degree += 1;
    
    } else {
        data[dest_node_id] = {.in_degree = 1, .out_degree = 0, .children = {}};
    }
}

int Dag::Dag::max_out_degree(){
    int curr_max = 0;

    for(const auto&[node, node_data] : data){
        curr_max = std::max(curr_max, node_data.out_degree);
    }

    return curr_max;
}

/// @brief Combine heuristics to get dag score
/// @return 
int Dag::Dag::score(){   
    return max_out_degree();
}

void Dag::Dag::render_dag(const fs::path& current_circuit_dir){
    std::ostringstream dot_string;

    dot_string << "digraph G {\n";

    for(const Resource::Qubit& qubit : qubits){
        qubit.extend_dot_string(dot_string);
    }

    dot_string << "}\n";


    fs::path dag_render_path = current_circuit_dir / "dag.png";

    const std::string str = dag_render_path.string();
    std::string command = "dot -Tpng -o " + str;
    
    pipe_to_command(command, dot_string.str());
    INFO("Program DAG rendered to " + dag_render_path.string());
}