#ifndef GRAPH_H
#define GRAPH_H

#include "utils.h"
#include <fstream>
#include "assert.h"
#include <limits>

class Graph {

    public:
        Graph(){}

        Graph(int _vertices) :
        vertices(_vertices), 
        possible_pairs(n_choose_2(vertices)), 
        graph(vertices, std::vector<int>(vertices, 0)), 
        shortest_distances(vertices, std::vector<int>(vertices, 0)){
            assert(vertices >= 2);
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

        std::pair<int, int> get_best_edge();

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

        void write_dot_file(const std::string& filename);

    private:
        int vertices = 0;
        std::vector<std::pair<int, int>> possible_pairs;
        std::vector<std::vector<int>> graph;
        std::vector<std::vector<int>> shortest_distances;
};

#endif
