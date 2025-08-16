#ifndef DAG_H
#define DAG_H

#include <node.h>
#include <collection.h>


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
    class Dag {
        
        public:
            Dag(){}

            void make_dag(const Collection<Resource::Qubit>& _qubits);

            /// @brief Given 2 DAGs create child DAG that's a combination of both
            /// TODO: Make this use both dags to create child
            /// @param dag1 
            /// @param dag2 
            void make_dag(const Dag& dag1, const Dag& dag2);

            inline Collection<Resource::Qubit> get_qubits() const {
                return qubits;
            }

            void add_edge(int source_node_id, int dest_node_id);

            void render_dag(const fs::path& current_circuit_dir);

            int max_out_degree();

            int score();

            friend std::ostream& operator<<(std::ostream& stream, const Dag& dag){

                stream << "=========================================" << std::endl;
                stream << "                ADJ LIST                 " << std::endl; 
                stream << "=========================================" << std::endl;

                stream << "N_NODES: " << dag.n_nodes << std::endl;

                for(const auto&[node, node_data] : dag.data){

                    stream << node << " -> children: ";

                    for(const int& child : node_data.children){
                        stream << child << ", ";
                    }

                    stream << "in_degree: " << node_data.in_degree << ", out_degree: " << node_data.out_degree << std::endl;
                }

                return stream;
            }

        private:
            size_t n_nodes = 0;
            std::unordered_map<int, Node_data> data;
            Collection<Resource::Qubit> qubits;

    };

};

#endif
