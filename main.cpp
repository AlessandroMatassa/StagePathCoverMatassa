#include "graph.hpp"
#include "cover.hpp"
#include "blockDecompose.hpp" 
#include "wholeAlgoritmo.hpp"

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




int main() {

    // ===================== TEST BASE DAG =====================

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

    // ===================== GRAFO TESI =====================

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

    // ===================== COVER =====================

    CoverState state(tStar.nodes.size());

    // -------- FASE 1 --------
    auto initialPaths = Cover::computeInitialPathCover(tStar, state);

    std::cout << "\n=== DEBUG FLOW DA SOURCE ===\n";

int s = tStar.nodeIndex["global_source"];

for (auto& e : tStar.adj[s]) {
    auto& ed = state.getEdge(s, e.to);
    std::cout << tStar.nodes[s].name << " -> "
              << tStar.nodes[e.to].name
              << " flow=" << ed.flow << "\n";
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

    // -------- FASE 2 --------
    int source = tStar.nodeIndex["global_source"];
    int sink   = tStar.nodeIndex["global_sink"];

    Cover::reduceFlow(tStar, state, source, sink);

    // -------- FASE 3 --------
    auto finalPaths = Cover::extractFinalPaths(tStar, state, source, sink);

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

    

// ===================== TEST GRAFO PIU' COMPLETO =====================

        Graph z; //grafo un po' più significativo

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

  // -------- FASE 1 --------
    auto zinitialPaths = Cover::computeInitialPathCover(zStar, zstate);

    std::cout << "\n=== DEBUG FLOW DA SOURCE ===\n";

int sz = zStar.nodeIndex["global_source"];

for (auto& e : zStar.adj[sz]) {
    auto& ed = zstate.getEdge(sz, e.to);
    std::cout << zStar.nodes[sz].name << " -> "
              << zStar.nodes[e.to].name
              << " flow=" << ed.flow << "\n";
}

    std::cout << "\n=== PATH COVER INIZIALE ===\n\n";

    int count8 = 1;
    for (auto& path : zinitialPaths) {
        std::cout << "Path " << count8++ << ": ";
        printPath(zStar, path);
    }

    std::cout << "\n=== DEBUG INIZIALE (G*) ===\n\n";

    int count5 = 1;
    for (auto& path : zinitialPaths) {
        std::cout << "Path " << count5++ << ": ";
        printPath2(zStar, path);
    }

    // -------- FASE 2 --------
    int zsource = zStar.nodeIndex["global_source"];
    int zsink   = zStar.nodeIndex["global_sink"];

    Cover::reduceFlow(zStar, zstate, zsource, zsink);

    // -------- FASE 3 --------
    auto zfinalPaths = Cover::extractFinalPaths(zStar, zstate, zsource, zsink);

    std::cout << "\n=== MINIMUM PATH COVER ===\n\n";

    int count6 = 1;
    for (auto& path : zfinalPaths) {
        std::cout << "Path " << count6++ << ": ";
        printPath(zStar, path);
    }

    std::cout << "\n=== DEBUG FINALE (G*) ===\n\n";

    int count7 = 1;
    for (auto& path : zfinalPaths) {
        std::cout << "Path " << count7++ << ": ";
        printPath2(zStar, path);
    }


std::cout << "\n================ BLOCK DECOMPOSITION TEST ================\n";

// ===================== 1️ CONVERSIONE MPC =====================

auto cleanMPC = Cover::convertMPCtoOriginalGraph(zStar, z, zfinalPaths);

// ===================== 2️ TOPO SORT + MAPPING =====================

auto topo = topologicalSort(z);

std::vector<int> topoIndex(z.nodes.size());
for (int i = 0; i < topo.size(); i++) {
    topoIndex[topo[i]] = i;
}

// ===================== 3️ MATRICE DELTA =====================

auto delta = matriceBinaria(z, cleanMPC, topo);

int num_paths = delta.size();
int num_nodes = topo.size();

std::cout << "\n[INFO] Delta matrix size: "
          << num_paths << " x " << num_nodes << "\n";

// ===================== 4️ NODI IN ORDINE TOPO =====================

std::cout << "\n[INFO] Nodes in topological order:\n";

for (int i = 0; i < topo.size(); i++) {
    std::cout << i << ":" << z.nodes[topo[i]].name << "  ";
}
std::cout << "\n";

// ===================== 5️ STRINGA z (BINARIA) =====================

std::vector<int> z_string(num_nodes, 0);

// z = A B F G N I
z_string[topoIndex[z.nodeIndex["A"]]] = 1;
z_string[topoIndex[z.nodeIndex["B"]]] = 1;
z_string[topoIndex[z.nodeIndex["F"]]] = 1;
z_string[topoIndex[z.nodeIndex["G"]]] = 1;
z_string[topoIndex[z.nodeIndex["N"]]] = 1;
z_string[topoIndex[z.nodeIndex["I"]]] = 1;


/* z = "A","F", N"I"
z_string[topoIndex[z.nodeIndex["A"]]] = 1;
z_string[topoIndex[z.nodeIndex["F"]]] = 1;
z_string[topoIndex[z.nodeIndex["N"]]] = 1;
z_string[topoIndex[z.nodeIndex["I"]]] = 1;
*/
/*
// z = "A","J","P","M","I"
z_string[topoIndex[z.nodeIndex["A"]]] = 1;
z_string[topoIndex[z.nodeIndex["J"]]] = 1;
z_string[topoIndex[z.nodeIndex["P"]]] = 1;
z_string[topoIndex[z.nodeIndex["M"]]] = 1;
z_string[topoIndex[z.nodeIndex["I"]]] = 1;
*/
/*/ z =    {"A","D","C","M","I"});
z_string[topoIndex[z.nodeIndex["A"]]] = 1;
z_string[topoIndex[z.nodeIndex["D"]]] = 1;
z_string[topoIndex[z.nodeIndex["C"]]] = 1;
z_string[topoIndex[z.nodeIndex["M"]]] = 1;
z_string[topoIndex[z.nodeIndex["I"]]] = 1;
*/

// ===================== STAMPA Z =====================

std::cout << "\n[INFO] z (nodi attivi):\n";
for (int i = 0; i < num_nodes; i++) {
    if (z_string[i] == 1) {
        std::cout << z.nodes[topo[i]].name << " ";
    }
}
std::cout << "\n";

// =====================  DEBUG BINARIO =====================

std::cout << "\n[DEBUG] Delta (binaria)\n";

for (int p = 0; p < num_paths; p++) {
    std::cout << "Path " << p << ": ";
    for (int j = 0; j < num_nodes; j++) {
        std::cout << delta[p][j] << " ";
    }
    std::cout << "\n";
}

std::cout << "\n[DEBUG] z (binaria)\n";
for (int i = 0; i < num_nodes; i++) {
    std::cout << z_string[i] << " ";
}
std::cout << "\n";

// =====================  DELTA  =====================

std::cout << "\n[INFO] Delta (per path)\n";

for (int p = 0; p < num_paths; p++) {

    std::cout << "Path " << p << ": ";

    for (int j = 0; j < num_nodes; j++) {
        if (delta[p][j] == 1) {
            std::cout << z.nodes[topo[j]].name << " ";
        }
    }

    std::cout << "\n";
}

// ===================== 7️BLOCK DECOMPOSE =====================

BlockDecompose bd;
auto result = bd.BlocksDecompose(z_string, delta, num_paths);

// ===================== 8️ MATCH  =====================

std::cout << "\n[RESULT] Matched path per colonna:\n";

for (int i = 0; i < result.path.size(); i++) {
    std::cout << result.path[i] << " ";
}
std::cout << "\n";


std::cout << "\n[RESULT] Matching (solo nodi attivi):\n";

for (int i = 0; i < num_nodes; i++) {

    if (z_string[i] == 1) {

        std::cout << z.nodes[topo[i]].name
                  << " -> Path " << result.path[i] << "\n";
    }
}

// =====================  BLOCCHI =====================

std::cout << "\n[RESULT] Blocchi:\n";

for (auto& b : result.Blocks) {

    std::cout << "Path " << b.path
              << " per " << b.length << " nodi\n";
}

// =====================  BLOCCHI BINARI =====================

std::cout << "\n[DEBUG] Blocchi (binari):\n";

int i = 0;

while (i < result.path.size()) {

    if (result.path[i] == -1) {
        i++;
        continue;
    }

    int currentPath = result.path[i];

    std::cout << "Path " << currentPath << ": ";

    while (i < result.path.size() && result.path[i] == currentPath) {

        std::cout << z_string[i] << " ";
        i++;
    }

    std::cout << "\n";
}

std::cout << "\n==========================================================\n";



//cose per parser

auto parsed = readGFA("E-3133.gfa");   
Graph& gfaGraph = parsed.g;

// ===================== INFO GENERALE =====================

std::cout << "\n[INFO] Numero nodi: " << gfaGraph.nodes.size() << "\n";
std::cout << "[INFO] Numero archi: ";

int edgeCount = 0;
for (auto& list : gfaGraph.adj)
    edgeCount += list.size();

std::cout << edgeCount << "\n";

// ===================== STAMPA NODI =====================

std::cout << "\n[DEBUG] Nodi:\n";

for (int i = 0; i < gfaGraph.nodes.size(); i++) {
    std::cout << i << " : " << gfaGraph.nodes[i].name << "\n";
}

// ===================== STAMPA ARCHI =====================

std::cout << "\n[DEBUG] Archi:\n";

for (int u = 0; u < gfaGraph.nodes.size(); u++) {

    std::cout << gfaGraph.nodes[u].name << " -> ";

    for (auto& e : gfaGraph.adj[u]) {
        std::cout << gfaGraph.nodes[e.to].name << " ";
    }

    std::cout << "\n";
}


std::cout << "\nFILE GFA ORIGINALE\n";


  std::ifstream file1("E-3133.gfa");

    if (!file1.is_open()) {
        std::cerr << "Errore nell'apertura del file" << std::endl;
        return 1;
    }

    std::string line1;

    while (std::getline(file1, line1)) {
        if (line1.empty()) continue; // salta righe vuote
        std::cout << line1 << std::endl;
    }

    file1.close();





std::cout << "\nFILE GFA MODIFICATO\n";

//COSA DEFINITIVA
wholeAlgoritmo::GFAFinale("E-3133.gfa");
//stampiamo il file arricchito
  std::ifstream file("output.gfa");

    if (!file.is_open()) {
        std::cerr << "Errore nell'apertura del file" << std::endl;
        return 1;
    }

    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue; // salta righe vuote
        std::cout << line << std::endl;
    }

    file.close();

return 0;

}