#ifndef GRAPH_H
#define GRAPH_H

#include "utils.h"

class Graph {

    public:
        Graph(){}

        Graph(int _vertices) : vertices(_vertices), graph(vertices, std::vector<int>(vertices, 0)), shortest_distances(vertices, std::vector<int>(vertices, 0)){
            for(int i = 0; i < vertices; i++){
                for(int j = i+1; j < vertices; j++){
                    num_pairs += 1;
                }
            }
        }

        std::vector<int> djikstras(int source_node);

        void shortest_distance_between_all_pairs(){
            for(int i = 0; i < vertices; i++){
                shortest_distances[i] = std::move(djikstras(i));
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

        void reset(){
            for(int i = 0; i < vertices; i++){
                for(int j = 0; j < vertices; j++){
                    graph[i][j] = 0;
                    graph[j][i] = 0;
                }
            }
        }

        void write_dot_file(const std::string& filename);

    private:
        int vertices = 0;
        int num_pairs = 0;
        std::vector<std::vector<int>> graph;
        std::vector<std::vector<int>> shortest_distances;
};

#endif
