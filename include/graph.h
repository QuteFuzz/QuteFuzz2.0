#ifndef GRAPH_H
#define GRAPH_H

#include "utils.h"
#include "qubit.h"
#include <fstream>
#include <limits>

class Graph {

    public:
        Graph(){}

        Graph(int _vertices) :
        vertices(_vertices), 
        num_pairs((vertices * (vertices - 1)) / 2),
        graph(vertices, std::vector<int>(vertices, 0)), 
        shortest_distances(vertices, std::vector<int>(vertices, 0)){
            assert(vertices >= Common::MIN_N_QUBITS_IN_ENTANGLEMENT);
        }

        std::vector<int> djikstras(int source_node);

        void shortest_distance_between_all_pairs(){
            for(int i = 0; i < vertices; i++){
                shortest_distances[i] = std::move(djikstras(i));
            }
        }

        void add_entanglement(const std::vector<int>& entanglement, const std::vector<std::vector<int>>& edges){
            for(const auto& edge : edges){
                add_edge(entanglement[edge[0]], entanglement[edge[1]]);
            }
        }

        void remove_entanglement(const std::vector<int>& entanglement, const std::vector<std::vector<int>>& edges){
            for(const auto& edge : edges){
                remove_edge(entanglement[edge[0]], entanglement[edge[1]]);
            }
        }

        void add_edge(int u, int v){
            if((u < vertices) && (v < vertices)){
                graph[u][v] += 1;
                graph[v][u] += 1;
            }
        }

        void remove_edge(int u, int v){
            if((u < vertices) && (v < vertices)){
                graph[u][v] -= 1;
                graph[v][u] -= 1;
            }
        }

        int ASP();

        int diameter();

        int score();

        std::vector<int> get_best_entanglement(int n_qubits_in_entanglement);

        void reset(){
            for(int i = 0; i < vertices; i++){
                for(int j = 0; j < vertices; j++){
                    graph[i][j] = 0;
                    graph[j][i] = 0;
                }
            }
        }

        friend std::ostream& operator<<(std::ostream& stream, Graph g){
            for(int i = 0; i < g.vertices; i++){
                for(int j = 0; j < g.vertices; j++){
                    std::cout << g.graph[i][j] << " ";
                }
                std::cout << "\n";
            }

            return stream;
        }

        void write_dot_file(fs::path& dot_path, std::shared_ptr<Qreg_definitions> current_defs);

    private:
        int vertices = 0;
        int num_pairs = 0;
        std::vector<std::vector<int>> graph;
        std::vector<std::vector<int>> shortest_distances;
};

#endif
