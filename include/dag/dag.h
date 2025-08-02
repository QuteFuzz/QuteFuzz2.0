#ifndef DAG_H
#define DAG_H

#include <node.h>

template<typename T>
struct Collection;

namespace Qubit {
    class Qubit;
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

    /// @brief Given a set of qubits, get DAG score using the path taken by each qubit
    class Process {
        
        public:
            Process(){}

            /// @brief Use qubit paths to construct useful data structures used to calculate graph theoretic metrics
            /// @param qubits 
            Process(const Collection<Qubit::Qubit>& qubits);

            friend std::ostream& operator<<(std::ostream& stream, const Process& process){

                stream << "=========================================" << std::endl;
                stream << "                ADJ LIST                 " << std::endl; 
                stream << "=========================================" << std::endl;

                stream << "N_NODES: " << process.n_nodes << std::endl;

                for(const auto&[node, neighbours] : process.adj_list){

                    stream << node << ": ";

                    for(const int& neighbour : neighbours){
                        stream << neighbour << ", ";
                    }

                    stream << std::endl;
                }

                return stream;
            }

            int score();

        private:
            size_t n_nodes;

            std::unordered_map<int, std::vector<int>> adj_list;

    };

};

#endif
