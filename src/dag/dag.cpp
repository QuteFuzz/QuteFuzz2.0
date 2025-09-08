#include <resource.h>
#include <collection.h>
#include <dag.h>

void Dag::Dag::make_dag(const Collection<Resource::Qubit>& _qubits, const Collection<Resource::Bit>& _bits, std::string _name){
    reset();

    qubits = _qubits;
    bits = _bits;
    name = _name;

    for(const Resource::Qubit& qubit : qubits){
        qubit.add_path_to_dag(*this);
    }
}

std::optional<unsigned int> Dag::Dag::nodewise_data_contains(std::shared_ptr<Qubit_op> node){
    
    for(unsigned int i = 0; i < nodewise_data.size(); i++){
        if(nodewise_data[i].node->get_id() == node->get_id()){
            return std::make_optional<unsigned int>(i);   
        }
    }

    return std::nullopt;
}


void Dag::Dag::add_edge(const Edge& edge, std::optional<int> maybe_dest_node_id, int qubit_id){

    unsigned int source_node_input_port = edge.get_dest_port();
    std::shared_ptr<Qubit_op> source_node = edge.get_node();

    source_node->set_from_dag();

    std::optional<unsigned int> maybe_pos = nodewise_data_contains(source_node);
    unsigned int pos = maybe_pos.value_or(nodewise_data.size());

    if(maybe_pos.has_value() == false){
        nodewise_data.push_back(Node_data{.node = source_node, .inputs = {}, .children = {}});
        
        // reserve memory for inputs depending on number of ports this gate has
        nodewise_data.at(pos).inputs.resize(source_node->get_n_ports(), 0);
    }

    if(maybe_dest_node_id.has_value()) nodewise_data.at(pos).children.push_back(maybe_dest_node_id.value());

    nodewise_data.at(pos).inputs[source_node_input_port] = qubit_id;

    source_node->add_gate_if_subroutine(subroutine_gates);
}

int Dag::Dag::max_out_degree(){
    unsigned int curr_max = 0;

    for(const auto&data : nodewise_data){
        curr_max = std::max(curr_max, data.out_degree());
    }

    return curr_max;
}

/// @brief Combine heuristics to get dag score
/// @return 
int Dag::Dag::score(){   
    return max_out_degree();
}

void Dag::Dag::render_dag(const fs::path& current_circuit_dir, const std::string dag_name){
    std::ostringstream dot_string;

    dot_string << "digraph G {\n";

    for(const Resource::Qubit& qubit : qubits){
        qubit.extend_dot_string(dot_string);
    }

    dot_string << "}\n";

    fs::path dag_render_path = current_circuit_dir / (dag_name + ".png");

    const std::string str = dag_render_path.string();
    std::string command = "dot -Tpng -o " + str;
    
    pipe_to_command(command, dot_string.str());
    INFO("Program DAG rendered to " + YELLOW(dag_render_path.string()));
}