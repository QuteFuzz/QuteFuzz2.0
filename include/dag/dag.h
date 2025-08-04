#ifndef DAG_H
#define DAG_H

#include <node.h>

template<typename T>
struct Collection;

namespace Qubit {
    class Qubit;
}

namespace Bit {
    class Bit;
}

namespace Dag {

    class Edge {

        public:
            Edge(){}

            Edge(size_t _src_port, size_t _dest_port, std::shared_ptr<Node> _node): 
                src_port(_src_port),
                dest_port(_dest_port),
                node(_node)
            {}

            size_t get_dest_port(){
                return dest_port;
            }

            std::string get_node_resolved_name() const {
                return "\"" + node->resolved_name() + "\"";
            }

            int get_node_id() const {
                return node->get_id();
            }

            friend std::ostream& operator<<(std::ostream& stream, const Edge& edge) {
                stream << " -> " << edge.get_node_resolved_name() << "[label=\"" << std::to_string(edge.src_port) << ", " << std::to_string(edge.dest_port) << "\"";
                return stream;
            }

        private:
            size_t src_port;
            size_t dest_port;
            std::shared_ptr<Node> node;
    };

    struct Node_data{
        int in_degree;
        int out_degree;
        std::vector<int> children;
    };

    /// @brief Given a set of qubits, get DAG score using the path taken by each qubit
    class Heuristics {
        
        public:
            Heuristics(){}

            /// @brief Use qubit paths to construct useful data structures used to calculate graph theoretic metrics
            /// @param qubits 
            Heuristics(const Collection<Qubit::Qubit>& qubits, const Collection<Bit::Bit>& bits);

            void add_edge(int source_node_id, int dest_node_id){
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

            friend std::ostream& operator<<(std::ostream& stream, const Heuristics& h){

                stream << "=========================================" << std::endl;
                stream << "                ADJ LIST                 " << std::endl; 
                stream << "=========================================" << std::endl;

                stream << "N_NODES: " << h.n_nodes << std::endl;

                for(const auto&[node, node_data] : h.data){

                    stream << node << " -> children: ";

                    for(const int& child : node_data.children){
                        stream << child << ", ";
                    }

                    stream << "in_degree: " << node_data.in_degree << ", out_degree: " << node_data.out_degree << std::endl;
                }

                return stream;
            }

            int max_out_degree(){
                int curr_max = 0;

                for(const auto&[node, node_data] : data){
                    curr_max = std::max(curr_max, node_data.out_degree);
                }

                return curr_max;
            }

            int score();

        private:
            size_t n_nodes = 0;
            std::unordered_map<int, Node_data> data;

    };

};

#endif
