#ifndef DAG_H
#define DAG_H

#include <node.h>
#include <collection.h>
#include <compound_stmt.h>

namespace Dag {

    class Edge {

        public:
            Edge(){}

            Edge(unsigned int _src_port, unsigned int _dest_port, std::shared_ptr<Compound_stmt> _node): 
                src_port(_src_port),
                dest_port(_dest_port),
                node(_node)
            {}

            inline std::shared_ptr<Compound_stmt> get_node() const {
                return node;
            }

            inline unsigned int get_dest_port() const {
                return dest_port;
            }

            std::string get_node_resolved_name() const;

            int get_node_id() const;

            friend std::ostream& operator<<(std::ostream& stream, const Edge& edge) {
                stream << " -> " << edge.get_node_resolved_name() << "[label=\"" << std::to_string(edge.src_port) << ", " << std::to_string(edge.dest_port) << "\"";
                return stream;
            }

        private:
            unsigned int src_port;
            unsigned int dest_port;
            std::shared_ptr<Compound_stmt> node;
    };

    struct Node_data{
        std::shared_ptr<Compound_stmt> node;

        std::vector<unsigned int> inputs; // each index is a port into the node, the value at each port is the id for the qubit entering that port
        std::vector<unsigned int> children; // node ids of child nodes

        inline unsigned int in_degree() const {
            return inputs.size();
        }

        inline unsigned int out_degree() const {
            return children.size();
        }
    };

    /// @brief Given a set of qubits, get DAG score using the path taken by each qubit
    class Dag {
        
        public:
            Dag(){}

            void make_dag(const Collection<Resource::Qubit>& _qubits);

            inline Collection<Resource::Qubit> get_qubits() const {
                return qubits;
            }

            void add_edge(const Edge& edge, std::optional<int> maybe_dest_node_id, int qubit_id);

            void render_dag(const fs::path& current_circuit_dir);

            int max_out_degree();

            int score();
            
            /// @brief Each node is a compound statement, so just count nodes to tell you how many you need in the AST for the main circuit
            /// @return 
            inline unsigned int n_compound_statements() const {
                return n_nodes;
            }

            unsigned int n_subroutines() const {
               return subroutine_gates.size();
            }

            inline std::shared_ptr<Compound_stmt> get_next_node(){
                if(node_pointer < n_nodes){
                    return nodewise_data.at(node_pointer++).node;
                } else {
                    return dummy_node;
                }
            }

            inline std::shared_ptr<Gate> get_next_subroutine_gate(){
                if(sub_pointer < subroutine_gates.size()){
                    return subroutine_gates.at(sub_pointer++);
                } else {
                    return dummy_gate;
                }
            }

            std::optional<unsigned int> nodewise_data_contains(std::shared_ptr<Compound_stmt> node);

            friend std::ostream& operator<<(std::ostream& stream, const Dag& dag){

                stream << "=========================================" << std::endl;
                stream << "                DAG INFO                 " << std::endl; 
                stream << "=========================================" << std::endl;

                stream << "N_NODES: " << dag.n_nodes << std::endl;
                stream << "N_SUBROUTINES: " << dag.n_subroutines() << std::endl;

                for(const auto& node_data : dag.nodewise_data){

                    stream << node_data.node->get_id() << " -> children: ";

                    for(const unsigned int& child_id : node_data.children){
                        stream << child_id << ", ";
                    }

                    stream << "in_degree: " << node_data.in_degree() << ", out_degree: " << node_data.out_degree() << std::endl;
                }

                return stream;
            }

            inline void reset(){
                n_nodes = 0;
                nodewise_data.clear();
                node_pointer = 0;
                subroutine_gates.clear();
                sub_pointer = 0;
            }

        private:
            unsigned int n_nodes = 0;
            std::vector<Node_data> nodewise_data;
            unsigned int node_pointer = 0;
            std::vector<std::shared_ptr<Gate>> subroutine_gates;
            unsigned int sub_pointer = 0;

            Collection<Resource::Qubit> qubits;

            std::shared_ptr<Resource::Qubit> dummy_qubit;
            std::shared_ptr<Compound_stmt> dummy_node;
            std::shared_ptr<Gate> dummy_gate;
    };

};

#endif
