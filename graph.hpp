#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <string>

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

    int demand = 0;
    int flow = 0;
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
                 const std::string& to,
                 int demand = 0,
                 int flow = 0) {

        int u = addNode(from);
        int v = addNode(to);

        // arco uscente
        adj[u].push_back({v, demand, flow});

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

// reset flow
void resetFlow(Graph& g) {

    for (auto& list : g.adj)

        for (auto& e : list)

            e.flow = 0;
}

//non so ancora se mi serve in realtà
Graph deepCopy(Graph& g) {

    Graph copy;

    for (auto& node : g.nodes)
        copy.addNode(node.name);

    for (int u = 0; u < g.nodes.size(); u++)
        for (auto& e : g.adj[u])
            copy.addEdge(g.nodes[u].name, g.nodes[e.to].name, e.demand, e.flow);

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
        gStar.addEdge(m, p, 1, 0);

    }
    for(int u = 0; u < g.nodes.size(); u++) // per ogni nodo u del grafo originale 
    {
        for(auto &e : g.adj[u]) // per ogni arco e uscente da u
        {
            gStar.addEdge(g.nodes[u].name + "p", g.nodes[e.to].name + "m", 0, 0);
        }
    }
gStar.addNode("global_source");
gStar.nodes[gStar.nodes.size() - 1].setColor(1);
gStar.addNode("global_sink");
gStar.nodes[gStar.nodes.size() - 1].setColor(1);
for(int u = 0; u < g.nodes.size(); u++) //questo ciclo aggiunge un arco da global_source a Am e da Ap a global_sink per ogni nodo u del grafo originale
{
    gStar.addEdge("global_source", g.nodes[u].name + "m", 0, 0);
    gStar.addEdge(g.nodes[u].name + "p", "global_sink", 0, 0);
}


    
    return gStar;

   // da fare

}


// parser
Graph readGFA(const std::string& filename) {

   // ancora da fare

}


#endif