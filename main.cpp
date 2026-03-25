#include "graph.hpp"
#include "cover.hpp"
#include <iostream>

// stampa path leggibile (senza m/p)
void printPath(Graph& g, const std::vector<int>& path) 
{
    for (int i = 0; i < path.size(); i++) {
        std::string name = g.nodes[path[i]].name;
        // stampa solo i nodi "m" per evitare duplicati
        if (name.back() == 'm') {
            std::cout << name.substr(0, name.size() - 1);
            if (i < path.size() - 1)
                std::cout << " -> ";
        }
    }
    std::cout << "\n";
}


void printPath2(Graph& g, const std::vector<int>& path) 
{
    for (size_t i = 0; i < path.size(); i++) {
        std::cout << g.nodes[path[i]].name; // stampa il nome completo, incluso m/p
        if (i < path.size() - 1)
            std::cout << " -> ";
    }
    std::cout << "\n";
}

int main() {

    Graph g;

    // Costruiamo il grafo:
    //
    // A → C → D
    // B ↗

    g.addEdge("A","C");
    g.addEdge("B","C");
    g.addEdge("C","D");


    std::cout << "=== Lista archi (adjacency list) ===\n";

    for(int u = 0; u < g.nodes.size(); u++) {

        std::cout << g.nodes[u].name << " -> ";

        for(auto &e : g.adj[u])
            std::cout << g.nodes[e.to].name << " ";

        std::cout << "\n";
    }


    std::cout << "\n=== Predecessori ===\n";

    for(int i = 0; i < g.nodes.size(); i++) {

        std::cout << g.nodes[i].name << " <- ";

        for(int p : g.nodes[i].predecessors)
            std::cout << g.nodes[p].name << " ";

        std::cout << "\n";
    }


    std::cout << "\n=== Test DAG ===\n";

    if(isDAG(g))
        std::cout << "Il grafo e' un DAG\n";
    else
        std::cout << "Il grafo NON e' un DAG\n";


    Graph t;

    // ===== COSTRUISCO IL GRAFO DELL'IMMAGINE =====

    t.addEdge("A", "B");
    t.addEdge("A", "D");
 
    t.addEdge("B", "C");

    t.addEdge("D", "F");

    t.addEdge("F", "C");
    t.addEdge("F", "G");

    t.addEdge("C", "H");

    t.addEdge("H", "I");
    t.addEdge("G", "I");



    Graph tStar = convertGraph(t); //converto

    // ===== COVER =====
    // 
    auto paths = Cover::computeInitialPathCover(tStar);

    // ===== STAMPA =====

    std::cout << "Path cover trovata:\n\n";

    int count = 1;

    for (auto& path : paths) {

        std::cout << "Path " << count++ << ": ";
        printPath(tStar, path);
    }
std::cout << "Path cover trovata:\n\n";

int count2 = 1;

    for (auto& path : paths) {

        std::cout << "Path " << count2++ << ": ";
        printPath2(tStar, path);
    }
    return 0;
}


