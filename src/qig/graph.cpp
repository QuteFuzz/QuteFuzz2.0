#include <graph.h>

std::vector<int> Graph::djikstras(int source_node){

    std::vector<int> distances(vertices, __INT_MAX__);
    distances[source_node] = 0;

    std::queue<int> q;
    std::vector<int> visited(vertices); 
    std::vector<int> nbors;

    q.push(source_node); // we know that current node will have the minimum distance
    visited[source_node] = 1;

    int current_node, current_dist;

    while(!q.empty()){

        current_node = q.front();
        q.pop();

        nbors = graph[current_node];

        for(int i = 0; i < vertices; ++i){
            if(nbors[i] && !visited[i]){
                // legitimate neighbour that hasn't been visited
                q.push(i);
                visited[i] = 1;

                /*
                    this is basically saying that the shortest distance between node i and the source node = shortest dist between curr_node and source node + dist between curr_node and node i
                */
                current_dist = graph[i][current_node] + distances[current_node]; // graph[i][current_node] is weight between node off queue and its nbor, distances[curr_node] is shortest dist to source node
                distances[i] = std::min(distances[i], current_dist); 
            }
        }
    }

    // // print distances

    // for(size_t i = 0; i < vertices; ++i){
    //     std::cout << distances[i] << " ";
    // }

    // std::cout << std::endl;

    return distances;
}

/// @brief Average shortest path of the graph
/// @return 
int Graph::ASP(){

    int sum = 0;

    for(int i = 0; i < vertices; i++){
        sum += vector_sum(shortest_distances[i]);
    }

    return sum / num_pairs;
}

/// @brief Longest shortest path in the graph
/// @return 
int Graph::diameter(){

    int longest_path = 0;

    for(int i = 0; i < vertices; i++){
        longest_path = std::max(longest_path, vector_max(shortest_distances[i]));
    }

    return longest_path;
}


int Graph::score(){
    shortest_distance_between_all_pairs();

    return ASP() / diameter();

    // return 0;
}

std::vector<int> Graph::get_best_entanglement(int n_qubits_in_entanglement){
    int best_score = -INT32_MAX;

    std::vector<std::vector<int>> possible_entanglements = Common::QUBIT_COMBINATIONS.at(vertices, n_qubits_in_entanglement);
    std::vector<std::vector<int>> edges = Common::QUBIT_COMBINATIONS.at(n_qubits_in_entanglement, 2); 

    assert(!possible_entanglements.empty());
    assert(!edges.empty());
    std::vector<int> res = possible_entanglements[0];

	for(const std::vector<int>& entanglement : possible_entanglements){
        add_entanglement(entanglement, edges);

		int curr_score = score();

		if(curr_score > best_score){
			best_score = curr_score;
			res = entanglement;
		}

        remove_entanglement(entanglement, edges);
	}

    add_entanglement(res, edges);

    return res;
}


void Graph::render_graph(fs::path& img_path, std::shared_ptr<Block> block){    
    std::string dot_string;

    // create dot string    
    dot_string += "graph G {\n";
    int n = graph.size();
    for (int i = 0; i < n; ++i){
        for (int j = i+1; j < n; ++j){
            if (graph[i][j]) {
                std::string qubit_i = block->get_qubit_at(i)->resolved_name();
                std::string qubit_j = block->get_qubit_at(j)->resolved_name();

                dot_string += ("  " + qubit_i + " -- " + qubit_j + " [label=" + std::to_string(graph[i][j]) + ", color=\"red\", penwidth=3];\n");
            }
        }
    }
    dot_string += "}\n";

    // render graph
    const std::string str = img_path.string();
    std::string command = "dot -Tpng -o " + str;
    
    pipe_to_command(command, dot_string);
    INFO("QIG rendered to " + img_path.string());
}
