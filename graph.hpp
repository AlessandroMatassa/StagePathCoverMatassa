#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <string>

//in questa struct salvo i path del file gfa, in modo da poterli convertire in z e delta
struct GFAPath {
    std::string name;
    std::vector<int> nodes;
};


// Nodo
struct Node {
    std::string name;

    // DFS colors
    int color = 0; // 0 bianco, 1 grigio, 2 nero
// il tempo di ingresso serve per il controllo dei cicli, il tempo di uscita per l'ordinamento topologico
    int u = 0; // per DFS
    int m = 1; //

    // lista dei predecessori
    std::vector<int> predecessors;


    void setColor(int c) {
        color = c;
    }
};


// Arco
struct Edge {
    // il nodo di partenza è implicito nella lista di adiacenza
    int to;

   //int demand = 0;     ////////********** DA RIMUOVERE **********////////
    //int flow = 0;       ////////********** DA RIMUOVERE **********////////
};

// Grafo
class Graph {
public:

    std::vector<Node> nodes; // lista dei nodi
    std::vector<std::vector<Edge> > adj; // adj[u] = lista di archi uscente da u

    //
    std::unordered_map<std::string, int> nodeIndex;

    int addNode(const std::string& name) {

        if (nodeIndex.count(name))
            return nodeIndex[name];

        int index = nodes.size();

        nodeIndex[name] = index;

        Node n;
        n.name = name;

        nodes.push_back(n);
        adj.push_back({});

        return index;
    }

    void addEdge(const std::string& from,
                 const std::string& to)
                    
                 { 

        int u = addNode(from);
        int v = addNode(to);

        // evita duplicati
    for (auto& e : adj[u]) {
        if (e.to == v)
            return;
    }

        // arco uscente
        adj[u].push_back({v}); // i due zeri

        // aggiorna predecessori
        nodes[v].predecessors.push_back(u);
    }

    void clear() {
        nodes.clear();
        adj.clear();
        nodeIndex.clear();
    }
};

// DFS per rilevare cicli
bool dfsCycle(int u, std::vector<int>& color, Graph& g) {

    color[u] = 1;

    for (auto& e : g.adj[u]) {

        int v = e.to;

        if (color[v] == 1)
            return true;

        if (color[v] == 0 && dfsCycle(v, color, g))
            return true;
    }

    color[u] = 2;

    return false;
}

// controllo DAG
bool isDAG(Graph& g) {

    std::vector<int> color(g.nodes.size(), 0);

    for (int i = 0; i < g.nodes.size(); i++) {

        if (color[i] == 0 && dfsCycle(i, color, g)) {
            return false;
        }
    }

    return true;
}


// topological sort
std::vector<int> topologicalSort(Graph& g) {

std::vector<int> SortedElements;
std::queue<int> S; // set di nodi senza archi entranti
std::vector<int> inDegree(g.nodes.size(), 0);


 for(int u = 0; u < g.nodes.size(); u++)
    {
        for(auto &e : g.adj[u]) //per ogni arco e uscente da u, incrementa il grado di entrata del nodo di destinazione dell'arco e
        {
            inDegree[e.to]++;
        }
    }

    // nodi senza archi entranti
    for(int i = 0; i < g.nodes.size(); i++) //
    {
        if(inDegree[i] == 0)
            S.push(i);
    }

while(!S.empty())
{
    
    int n = S.front();
    SortedElements.push_back(n);
     S.pop();
    for(auto &e : g.adj[n]) //per ogni nodo m con un arco e da n a m
    {
        int m = e.to; // m è il nodo di destinazione dell'arco e
        inDegree[m]--;
        if(inDegree[m] == 0)
            S.push(m);
    }
}
if(SortedElements.size() != g.nodes.size())
{
    std::cerr << "errore\n";
    return {};
}
return SortedElements;
}

void resetPredecessor(Graph& g) {

    for (auto& node : g.nodes) {

        node.predecessors.clear();
    }
}

/* reset flow       ////////********** DA RIVEDERE 
void resetFlow(Graph& g) {

    for (auto& list : g.adj)

        for (auto& e : list)

            e.flow = 0;
} */

//non so ancora se mi serve in realtà
Graph deepCopy(Graph& g) {

    Graph copy;

    for (auto& node : g.nodes)
        copy.addNode(node.name);

    for (int u = 0; u < g.nodes.size(); u++)
        for (auto& e : g.adj[u])
            copy.addEdge(g.nodes[u].name, g.nodes[e.to].name); ////////********** DA RIMUOVERE **********////////

    return copy;
}
//converte il grafo in un grafo G star
Graph convertGraph(Graph& g) {

    //inizializzo un nuovo grafo G star
    Graph gStar;

    //sdoppia ogni nodo in Am e Ap, Am ha solo nodi uscendi e Ap solo nodi entranti, aggiunge un arco da Am a Ap con capacità pari alla domanda del nodo
    for(int u = 0; u < g.nodes.size(); u++)
    {

        std::string m = g.nodes[u].name + "m";
        std::string p = g.nodes[u].name + "p";
        gStar.addNode(m);
        //setto il colore
            gStar.nodes[gStar.nodes.size() - 1].setColor(1);
        gStar.addNode(p);
            gStar.nodes[gStar.nodes.size() - 1].setColor(1);
        gStar.addEdge(m, p );

    }
    for(int u = 0; u < g.nodes.size(); u++) // per ogni nodo u del grafo originale 
    {
        for(auto &e : g.adj[u]) // per ogni arco e uscente da u
        {
            gStar.addEdge(g.nodes[u].name + "p", g.nodes[e.to].name + "m");
        }
    }
gStar.addNode("global_source");
gStar.nodes[gStar.nodes.size() - 1].setColor(1);
gStar.addNode("global_sink");
gStar.nodes[gStar.nodes.size() - 1].setColor(1);
for(int u = 0; u < g.nodes.size(); u++) //questo ciclo aggiunge un arco da global_source a Am e da Ap a global_sink per ogni nodo u del grafo originale
{
    gStar.addEdge("global_source", g.nodes[u].name + "m");
    gStar.addEdge(g.nodes[u].name + "p", "global_sink");
}


    
    return gStar;

   // da fare

}
struct GFAGraph {
    Graph g;
    std::vector<GFAPath> paths;
};



// parser Da fare
GFAGraph readGFA(const std::string& filename) {

    GFAGraph result;
    Graph& g = result.g;

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Errore apertura file\n";
        return result;
    }

    std::string line;

    while (std::getline(file, line)) {

        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string type;
        ss >> type;

        // =====================
        // NODI
        // =====================
        if (type == "S") {

            std::string name;
            ss >> name; 

            g.addNode(name);
        }

        // =====================
        // ARCHI
        // =====================
        else if (type == "L") {

            std::string from, fromOrient;
            std::string to, toOrient;
            std::string overlap;

            ss >> from >> fromOrient >> to >> toOrient >> overlap;

            if (!from.empty() && !to.empty())
                g.addEdge(from, to);
        }

        
        // PATH
        else if (type == "P") {

            std::string pathName, segmentList, X;
            ss >> pathName >> segmentList >> X;

            GFAPath p;
            p.name = pathName;

            std::stringstream segStream(segmentList); // segmentList è una stringa del tipo "A+,B-,C+" che rappresenta i nodi del path e il loro orientamento, segStream serve a scomporre questa stringa nei singoli segmenti (A+, B-, C+)
            std::string token; // token memorizza temporaneamente ogni segmento scomposto da segStream

            while (std::getline(segStream, token, ',')) { //Finché riesce a scomporre segmentList in token usando la virgola come delimitatore, continua a processare ogni token

                if (token.empty()) continue; //potrei anche toglierlo

                // rimuove orientamento (+ o -)
                std::string nodeName = token.substr(0, token.size() - 1);

                if (g.nodeIndex.count(nodeName)) {
                    p.nodes.push_back(g.nodeIndex[nodeName]);
                }
            }

            if (!p.nodes.empty())
                result.paths.push_back(p);
        }

        // ignora tutto il resto
        else {
            continue;
        }
    }

    return result;
}


static std::vector<std::vector<int>> convertGFAPathsToZ(
    Graph& g,
    const std::vector<GFAPath>& gfaPaths, std::vector<int> topo
)
{
    // da modificare assumere sia già topologico
   // auto topo = topologicalSort(g);

    int num_nodes = topo.size();

    std::vector<int> topoIndex(g.nodes.size()); 

    for (int i = 0; i < num_nodes; i++) {
        topoIndex[topo[i]] = i;
    }

    // COSTRUZIONE Z
    std::vector<std::vector<int>> Z_all;

    for (const auto& p : gfaPaths) {

        std::vector<int> z_string(num_nodes, 0);

        for (int node : p.nodes)
         {

                //penso di poterlo togliere
            if (node >= 0 && node < g.nodes.size()) {
                int col = topoIndex[node];
                z_string[col] = 1;
            }
        }

        Z_all.push_back(z_string);
    }

    return Z_all;
}






#endif