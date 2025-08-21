#ifndef DAG_H
#define DAG_H

#include <node.h>
#include <collection.h>


namespace Dag {

    class Edge {

        public:
            Edge(){}

            Edge(unsigned int _src_port, unsigned int _dest_port, std::shared_ptr<Node> _node): 
                src_port(_src_port),
                dest_port(_dest_port),
                node(_node)
            {}

            inline std::shared_ptr<Node> get_node() const {return node;}

            inline unsigned int get_dest_port() const {
                return dest_port;
            }

            inline std::string get_node_resolved_name() const {
                return "\"" + node->resolved_name() + "\"";
            }

            inline int get_node_id() const {
                return node->get_id();
            }

            friend std::ostream& operator<<(std::ostream& stream, const Edge& edge) {
                stream << " -> " << edge.get_node_resolved_name() << "[label=\"" << std::to_string(edge.src_port) << ", " << std::to_string(edge.dest_port) << "\"";
                return stream;
            }

        private:
            unsigned int src_port;
            unsigned int dest_port;
            std::shared_ptr<Node> node;
    };

    struct Node_data{
        std::shared_ptr<Node> node;

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

            /// @brief Given 2 DAGs create child DAG that's a combination of both
            /// TODO: Make this use both dags to create child
            /// @param dag1 
            /// @param dag2 
            void make_dag(const Dag& dag1, const Dag& dag2);

            inline Collection<Resource::Qubit> get_qubits() const {
                return qubits;
            }

            void add_edge(const Edge& edge, std::optional<int> maybe_dest_node_id, int qubit_id);

            void render_dag(const fs::path& current_circuit_dir);

            int max_out_degree();

            int score();
            
            /// @brief Each node is either a qubit op, or an if_stmt (TODO), both are compound statements
            /// @return 
            inline unsigned int n_compound_statements() const {
                return n_nodes;
            }

            /// @brief How many nodes are subroutines?
            /// @return 
            inline unsigned int n_subroutines() const {
                return subroutines.size();
            }

            /// @brief Get subroutine node from DAG to use to create definition in AST. If there's no subroutines in the DAG,
            /// the part of the AST where this function is called will never be reached, return dummy
            /// @return 
            std::shared_ptr<Node> get_next_subroutine(){
                return subroutines.size() ? subroutines.at(next_sub_pointer++) : dummy; 
            }

            std::shared_ptr<Resource::Qubit> get_next_node(){
                return n_nodes ? nodewise_data.at(next_node_pointer++) : dummy;
            }

            inline std::optional<unsigned int> nodewise_data_contains(std::shared_ptr<Node> node){
                
                for(unsigned int i = 0; i < nodewise_data.size(); i++){
                    if(nodewise_data[i].node->get_id() == node->get_id()){
                        return std::make_optional<unsigned int>(i);   
                    }
                }

                return std::nullopt;
            }

            friend std::ostream& operator<<(std::ostream& stream, const Dag& dag){

                stream << "=========================================" << std::endl;
                stream << "                DAG INFO                 " << std::endl; 
                stream << "=========================================" << std::endl;

                stream << "N_NODES: " << dag.n_nodes << std::endl;
                stream << "N_SUBROUTINES: " << dag.n_subroutines() << std::endl;
                stream << "N_COMPOUND_STATEMENTS: " << dag.n_compound_statements() << std::endl;

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
                subroutines.clear();
                next_sub_pointer = 0;
                next_node_pointer = 0;
            }

        private:
            unsigned int n_nodes = 0;
            std::vector<Node_data> nodewise_data;

            Collection<Resource::Qubit> qubits;
            std::shared_ptr<Resource::Qubit> dummy_qubit;
            unsigned int next_node_pointer = 0;

            std::vector<std::shared_ptr<Node>> subroutines;
            std::shared_ptr<Node> dummy;
            unsigned int next_sub_pointer = 0;

    };

};

#endif
