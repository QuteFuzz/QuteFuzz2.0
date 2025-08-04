#include <bit.h>

std::string Bit::Bit::resolved_name() const {

    if(is_register_def()){
        return get_name()->get_string() + "[" + get_index()->get_string() + "]";
    } else {
        return get_name()->get_string();
    }

}

void Bit::Bit::extend_flow_path(const std::shared_ptr<Node> node, size_t current_port){

    Dag::Edge edge;

    if(flow_path_length == 0){
        // bit being operated on for the first time
        edge = Dag::Edge(0, current_port, node);

    } else {
        edge = Dag::Edge(flow_path.back().get_dest_port(), current_port, node);
    }
    
    flow_path_length += 1;
    flow_path.push_back(edge);
}

void Bit::Bit::extend_dot_string(std::ostringstream& ss) const {
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

void Bit::Bit::add_path_to_heuristics(Dag::Heuristics& h) const {

    if(flow_path_length >= 2){

        for(size_t i = 0; i < flow_path_length-1; i++){
            int current_node_id = flow_path.at(i).get_node_id();
            int next_node_id = flow_path.at(i+1).get_node_id();

            h.add_edge(current_node_id, next_node_id);
        }
    }
}
