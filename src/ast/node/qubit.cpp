#include <resource.h>

void Resource::Qubit::extend_flow_path(const std::shared_ptr<Node> node, size_t current_port){

    Dag::Edge edge;

    if(flow_path_length == 0){
        // qubit being operated on for the first time
        edge = Dag::Edge(0, current_port, node);

    } else {
        edge = Dag::Edge(flow_path.back().get_dest_port(), current_port, node);
    }
    
    flow_path_length += 1;
    flow_path.push_back(edge);
}

void Resource::Qubit::extend_dot_string(std::ostringstream& ss) const {
    std::string input_node = "\"Input: " + resolved_name() + "\"";
    std::string output_node = "\"Output: " + resolved_name()+ "\"";

    ss << input_node << "[color=\"" << flow_path_colour << "\"];" << std::endl;
    ss << output_node << "[color=\"" << flow_path_colour << "\"];" << std::endl;

    if(flow_path_length){

        ss << input_node;

        for(size_t i = 0; i < flow_path_length; i++){
            if(i != 0) {
                ss << flow_path[i - 1].get_node_resolved_name();
            }
            
            ss << flow_path[i] << ", color=\"" << flow_path_colour << "\"];" << std::endl;
        }

        Dag::Edge last_path = flow_path[flow_path_length-1]; 

        ss << last_path.get_node_resolved_name() << "-> " << output_node 
        << "[label=\"" << std::to_string(last_path.get_dest_port()) 
        << ",0\", color=\"" << flow_path_colour << "\"];" << std::endl;

    } else {
        ss << input_node << "-> " << output_node << ";" << std::endl;

    }
}

void Resource::Qubit::add_path_to_dag(Dag::Dag& dag) const {

    if(flow_path_length >= 2){

        for(size_t i = 0; i < flow_path_length-1; i++){
            int current_node_id = flow_path.at(i).get_node_id();
            int next_node_id = flow_path.at(i+1).get_node_id();

            dag.add_edge(current_node_id, next_node_id);
        }
    }
}
