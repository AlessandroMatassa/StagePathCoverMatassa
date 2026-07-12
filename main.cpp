#include "graph.hpp"
#include "cover.hpp"
#include "blockDecompose.hpp"
#include "wholeAlgoritmo.hpp"
#include "statistics.hpp"
#include "metrics.hpp"
#include <iostream>
#include <fstream>
#include <string>



// stampa path leggibile (senza m/p)
void printPath(Graph& g, const std::vector<int>& path)
{
    for (int i = 0; i < path.size(); i++) {

        std::string name = g.nodes[path[i]].name;

        if (!name.empty() && name.back() == 'm') {

            std::cout << name.substr(0, name.size() - 1);

            if (i < path.size() - 1)
                std::cout << " -> ";
        }
    }

    std::cout << "\n";
}



// stampa completa (debug)
void printPath2(Graph& g, const std::vector<int>& path)
{
    for (size_t i = 0; i < path.size(); i++) {

        std::cout << g.nodes[path[i]].name;

        if (i < path.size() - 1)
            std::cout << " -> ";
    }

    std::cout << "\n";
}



int main()
{

// ======================================================
// TEST BASE DAG
// ======================================================

    Graph g;

    g.addEdge("A","C");
    g.addEdge("B","C");
    g.addEdge("C","D");

    std::cout << "=== Lista archi ===\n";

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



// ======================================================
// GRAFO TESI
// ======================================================

    Graph t;

    t.addEdge("A", "B");
    t.addEdge("A", "D");
    t.addEdge("B", "C");
    t.addEdge("D", "F");
    t.addEdge("F", "C");
    t.addEdge("F", "G");
    t.addEdge("C", "H");
    t.addEdge("H", "I");
    t.addEdge("G", "I");

    Graph tStar = convertGraph(t);



// ======================================================
// COVER
// ======================================================

    CoverState state(tStar.nodes.size());

    auto initialPaths =
        Cover::computeInitialPathCover(
            tStar,
            state
        );

    std::cout << "\n=== DEBUG FLOW DA SOURCE ===\n";

    int s = tStar.nodeIndex["global_source"];

    for (auto& e : tStar.adj[s]) {

        auto& ed = state.getEdge(s, e.to);

        std::cout
            << tStar.nodes[s].name
            << " -> "
            << tStar.nodes[e.to].name
            << " flow="
            << ed.flow
            << "\n";
    }


    std::cout << "\n=== PATH COVER INIZIALE ===\n\n";

    int count = 1;

    for (auto& path : initialPaths) {

        std::cout << "Path " << count++ << ": ";

        printPath(tStar, path);
    }


    std::cout << "\n=== DEBUG INIZIALE (G*) ===\n\n";

    int count2 = 1;

    for (auto& path : initialPaths) {

        std::cout << "Path " << count2++ << ": ";

        printPath2(tStar, path);
    }



    int source =
        tStar.nodeIndex["global_source"];

    int sink =
        tStar.nodeIndex["global_sink"];

    Cover::reduceFlow(
        tStar,
        state,
        source,
        sink
    );



    auto finalPaths =
        Cover::extractFinalPaths(
            tStar,
            state,
            source,
            sink
        );



    std::cout << "\n=== MINIMUM PATH COVER ===\n\n";

    int count3 = 1;

    for (auto& path : finalPaths) {

        std::cout << "Path " << count3++ << ": ";

        printPath(tStar, path);
    }



    std::cout << "\n=== DEBUG FINALE (G*) ===\n\n";

    int count4 = 1;

    for (auto& path : finalPaths) {

        std::cout << "Path " << count4++ << ": ";

        printPath2(tStar, path);
    }



// ======================================================
// TEST BLOCK DECOMPOSITION
// ======================================================

    Graph z;

    z.addEdge("A", "B");
    z.addEdge("A", "D");
    z.addEdge("B", "C");
    z.addEdge("D", "F");
    z.addEdge("F", "C");
    z.addEdge("F", "G");
    z.addEdge("C", "H");
    z.addEdge("H", "I");
    z.addEdge("G", "I");
    z.addEdge("A", "J");
    z.addEdge("A", "L");
    z.addEdge("J", "K");
    z.addEdge("L", "P");
    z.addEdge("P", "K");
    z.addEdge("P", "M");
    z.addEdge("K", "O");
    z.addEdge("M", "N");
    z.addEdge("N", "I");
    z.addEdge("O", "I");

    Graph zStar = convertGraph(z);

    CoverState zstate(zStar.nodes.size());

    auto zinitialPaths =
        Cover::computeInitialPathCover(
            zStar,
            zstate
        );



    int zsource =
        zStar.nodeIndex["global_source"];

    int zsink =
        zStar.nodeIndex["global_sink"];

    Cover::reduceFlow(
        zStar,
        zstate,
        zsource,
        zsink
    );



    auto zfinalPaths =
        Cover::extractFinalPaths(
            zStar,
            zstate,
            zsource,
            zsink
        );



    auto cleanMPC =
        Cover::convertMPCtoOriginalGraph(
            zStar,
            z,
            zfinalPaths
        );



    auto topo =
        topologicalSort(z);



    std::vector<int> topoIndex(
        z.nodes.size()
    );

    for (int i = 0; i < topo.size(); i++) {
        topoIndex[topo[i]] = i;
    }



    auto delta =
        matriceBinaria(
            z,
            cleanMPC,
            topo
        );



    int num_paths = delta.size();

    int num_nodes = topo.size();



    std::vector<int> z_string(
        num_nodes,
        0
    );

    z_string[topoIndex[z.nodeIndex["A"]]] = 1;
    z_string[topoIndex[z.nodeIndex["B"]]] = 1;
    z_string[topoIndex[z.nodeIndex["F"]]] = 1;
    z_string[topoIndex[z.nodeIndex["G"]]] = 1;
    z_string[topoIndex[z.nodeIndex["N"]]] = 1;
    z_string[topoIndex[z.nodeIndex["I"]]] = 1;



    BlockDecompose bd;

    auto result =
        bd.BlocksDecompose(
            z_string,
            delta,
            num_paths
        );



    std::cout << "\n[RESULT] Blocchi:\n";

    for (auto& b : result.Blocks) {

        std::cout
            << "Path "
            << b.path
            << " per "
            << b.length
            << " nodi\n";
    }



// ======================================================
// TEST E-3133.gfa
// ======================================================

    std::cout
        << "\n\n================ E-3133.gfa ================\n";



    wholeAlgoritmo::GFAFinale(
        "E-3133.gfa"
    );



    std::ifstream file(
        "E-3133_output.gfa"
    );

    if (!file.is_open()) {

        std::cerr
            << "Errore apertura E-3133_output.gfa\n";

        return 1;
    }

    std::string line;

    std::cout
        << "\n[OUTPUT GFA COMMENTATO]\n\n";

    while (std::getline(file, line)) {

        if (line.empty())
            continue;

        std::cout << line << "\n";
    }

    file.close();



    Statistics::exportCSV(
        "E-3133.gfa",
        "statistics.csv"
    );

    std::cout
        << "\n[STATISTICHE ESPORTATE]\n";

    std::cout
        << "File: statistics.csv\n";



// ======================================================
// TEST grafoPiccolo.gfa
// ======================================================

    std::cout
        << "\n\n================ GRAFO PICCOLO ================\n";



    wholeAlgoritmo::GFAFinale(
        "grafoPiccolo.gfa"
    );



    std::ifstream piccoloOut(
        "grafoPiccolo_output.gfa"
    );

    if (!piccoloOut.is_open()) {

        std::cerr
            << "Errore apertura grafoPiccolo_output.gfa\n";

        return 1;
    }

    std::string piccoloLine;

    std::cout
        << "\n[OUTPUT GFA COMMENTATO]\n\n";

    while (std::getline(piccoloOut, piccoloLine)) {

        if (piccoloLine.empty())
            continue;

        std::cout << piccoloLine << "\n";
    }

    piccoloOut.close();



    Statistics::exportCSV(
        "grafoPiccolo.gfa",
        "statistics_piccolo.csv"
    );

    std::cout
        << "\n[STATISTICHE ESPORTATE]\n";

    std::cout
        << "File: statistics_piccolo.csv\n";




// TEST grafoGrande.gfa


    std::cout
        << "\n\n================ GRAFO GRANDE ================\n";



    wholeAlgoritmo::GFAFinale(
        "grafoGrande.gfa"
    );



    std::ifstream grandeOut(
        "grafoGrande_output.gfa"
    );

    if (!grandeOut.is_open()) {

        std::cerr
            << "Errore apertura grafoGrande_output.gfa\n";

        return 1;
    }

    std::string grandeLine;

    std::cout
        << "\n[OUTPUT GFA COMMENTATO]\n\n";

    while (std::getline(grandeOut, grandeLine)) {

        if (grandeLine.empty())
            continue;

        std::cout << grandeLine << "\n";
    }

    grandeOut.close();



    Statistics::exportCSV(
        "grafoGrande.gfa",
        "statistics_grande.csv"
    );

    std::cout
        << "\n[STATISTICHE ESPORTATE]\n";

    std::cout
        << "File: statistics_grande.csv\n";



    return 0;
}