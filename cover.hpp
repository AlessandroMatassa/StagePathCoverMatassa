#ifndef COVER_H
#define COVER_H

#include "graph.hpp"
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <queue>


// Strutture base

//questo struct mi serve per tenere traccia del nodo precedente e se l'arco è forward o backward durante 
//la ricerca del path da source a sink nel grafo residuo
struct Parent {
    int prev; // nodo precedente (indice nella lista dei nodi)
    bool forward; // true se l'arco è forward, false se è backward
};
//questa struct mi serve per tenere traccia della demand e del flow di ogni arco durante l'esecuzione dell'algoritmo
struct EdgeState {
    int demand = 0;
    int flow = 0;
};

struct CoverState {
    std::vector<int> u;
    std::vector<int> covered;
    std::unordered_map<long long, EdgeState> edgeData; //
//il costruttore inizializza i vettori u e covered con 0 per ogni nodo del grafo
    CoverState(int n) {
        u.resize(n, 0);
        covered.resize(n, 0);
    }
// questa funzione mi serve per generare una chiave univoca per ogni arco (u, v) da usare nell'edgeData
    long long key(int u, int v) const {
        //viene creata una chiave univoca combinando u e v in un long long, con u nei primi 32 bit e v nei secondi 32 bit
        return (static_cast<long long>(u) << 32) | v;
    }
 // questa funzione mi serve per accedere allo stato di un arco (u, v) usando la chiave generata dalla funzione key
    EdgeState& getEdge(int u, int v) {
        return edgeData[key(u, v)];
    }
};

class Cover {
public:
//metodo che serve per block decompose
static std::vector<std::vector<int>> convertMPCtoOriginalGraph(
    Graph& gStar,
    Graph& gOriginal,
    const std::vector<std::vector<int>>& pathsGstar)
{
    std::vector<std::vector<int>> result;

    for (const auto& path : pathsGstar) {

        std::vector<int> clean;

        for (int v : path) {

            const std::string& name = gStar.nodes[v].name;

            // prendiamo solo nodi m
            if (!name.empty() && name.back() == 'm') {

                // rimuovo la m
                std::string originalName = name.substr(0, name.size() - 1);

                // recupero indice nel grafo originale
                int originalIndex = gOriginal.nodeIndex[originalName];

                clean.push_back(originalIndex);
            }
        }

        if (!clean.empty())
            result.push_back(clean);
    }

    return result;
}

static std::vector<int>buildGreedyTinyPath(const Graph& g,const std::vector<int>& topo,const std::vector<int>& tinyNodeScores)
{
    int n = g.nodes.size();

    std::vector<int> topoPos(n);

    for (int i = 0; i < topo.size(); i++)
    {
        topoPos[topo[i]] = i; // topoPos[v] mi dice la posizione di v nella topological sort
    }

    std::vector<int> best(n, 0);
    std::vector<int> next(n, -1); 

    // DP backward sul DAG
    for (auto it = topo.rbegin();it != topo.rend();++it)
    {
        int v = *it;

        int weight =tinyNodeScores[topoPos[v]]; //il peso di un nodo è dato dal suo tinyNodeScore, che è un punteggio che indica quanto è "tiny" il nodo, calcolato in base alla lunghezza del nodo e alla soglia per i tiny block

        best[v] = weight;

        for (const auto& e : g.adj[v])
        {
            int candidate =weight +best[e.to];

            if (candidate > best[v])
            {
                best[v] = candidate;
                next[v] = e.to;
            }
        }
        if (next[v] == -1 && !g.adj[v].empty())
        {
            next[v] = g.adj[v][0].to;
        }
    }

    // source migliore
    int start = -1;
    int bestScore = -1;

    for (int v = 0; v < n; v++)
    {
        if (g.nodes[v].predecessors.empty() && best[v] > bestScore) // se v è un nodo senza archi entranti e ha un punteggio migliore del migliore trovato finora, aggiorna il nodo di partenza e il punteggio migliore
        {
            bestScore = best[v];start = v;
        }
    }

    std::vector<int> path;

    int curr = start;

    while (curr != -1) // finché curr è un nodo valido, aggiungi curr al path e aggiorna curr al nodo successivo indicato da next[curr]
    {
        path.push_back(curr);

        curr = next[curr];
    }

    return path; // ritorna il path trovato, che è un path che massimizza la somma dei punteggi dei nodi lungo il path, dove i punteggi sono dati dai tinyNodeScores
}





static std::vector<std::vector<int>> computeInitialPathCover(Graph& g, CoverState& state) {

    std::vector<std::vector<int>> paths;
    std::vector<int> topo = topologicalSort(g);

    initializeDemands(g, state);

    while (true) {

        computeU(g, topo, state);

        int start = findBestStart(state);

        if (start == -1 || state.u[start] == 0)
            break;

        std::vector<int> path = extractPath(g, state, start);

        markCovered(state, path, g);

        paths.push_back(path);

        if (allCovered(state))
            break;
    }

    return paths;
}

// riduce il flow lungo tutti i path da source a sink finché è possibile, 
//decrementando il flow di 1 per ogni arco del path
static void reduceFlow(Graph& g, CoverState& state, int source, int sink) {

    while (true) {
        // costruisce il grafo residuo e cerca un path da source a sink
        std::vector<Parent> parent(g.nodes.size(), {-1, true}); //Vettore di Parent per tenere traccia del path trovato da source a sink nel grafo residuo
        //Se non esiste un path da source a sink, esce dal ciclo
        if (!findAugmentingPath(g, state, source, sink, parent))
            break;
//altrimenti, se esiste un path da source a sink, decrementa il flow di 1 per ogni arco del path
        augmentFlow(g, state, source, sink, parent);
    }
}

//*************+da cambiare il modo in cui si ottiene l'edge, la map non va beme****************

// estrae i path da source a sink finché è possibile, decrementando il flow di 1 per ogni arco del path
//ritorna una lista di path, dove ogni path è rappresentato come una lista di nodi
static std::vector<std::vector<int>> extractFinalPaths(Graph& g,
                                                       CoverState& state,
                                                       int source,
                                                       int sink) {

    std::vector<std::vector<int>> result;
    // finché esiste un path da source a sink con flow > 0, estrai il path e decrementa il flow di 1 per ogni arco del path
    while (true) {

        std::vector<int> path;
        int v = source;
        // segue il path da source a sink, sempre seguendo un arco con flow > 0
        while (v != sink) {
            // aggiunge v al path
            path.push_back(v);
            
            bool found = false; //found mi serve per tenere traccia se ho trovato un arco con flow > 0
            // cerca un arco uscente da v con flow > 0
            for (auto& e : g.adj[v]) {
                // se trova un arco con flow > 0, decrementa il flow di 1 e
                // aggiorna v al nodo di destinazione dell'arco
                auto& edge = state.getEdge(v, e.to);

                if (edge.flow > 0) {

                    edge.flow--;
                    v = e.to;
                    found = true;
                    break;
                }
            }

            //da testare
//se non si trova un arco con flow > 0, esce dal ciclo
            if (!found)
                break;
        }
// se il path è vuoto o non termina in sink, esce dal ciclo
        if (path.empty() || v != sink)
            break;
// altrimenti, se il path termina in sink, lo aggiunge alla lista dei risultati
        path.push_back(sink);
        //aggiunge il path alla lista dei risultati
        result.push_back(path);
    }

    return result;
}





private:

// calcola u[v] per ogni nodo v, partendo dai nodi senza archi uscenti e risalendo all'indietro
static void computeU(Graph& g,
                     const std::vector<int>& topo,
                     CoverState& state) {

    for (auto it = topo.rbegin(); it != topo.rend(); ++it) {

        int v = *it;
        int best = 0;

        for (auto& e : g.adj[v]) {
            best = std::max(best, state.u[e.to]);
        }

        int val = (state.covered[v] == 0 ? 1 : 0);
        state.u[v] = val + best;
    }
}

static int findBestStart(CoverState& state) {

    int best = -1;
    int node = -1;

    for (int i = 0; i < state.u.size(); i++) {
        if (state.u[i] > best) {
            best = state.u[i];
            node = i;
        }
    }

    return node;
}
//questo metodo estrae un path a partire da un nodo start 
//seguendo sempre l'arco con il valore u più alto, fino a 
//quando non si raggiunge un nodo senza archi uscenti o tutti gli archi uscenti hanno u=0
static std::vector<int> extractPath(Graph& g,
                                     CoverState& state,
                                     int start) {

    std::vector<int> path;
    int v = start;

    while (true) {

        path.push_back(v);

        int best = -1;
        int next = -1;

        for (auto& e : g.adj[v]) {
            if (state.u[e.to] > best) {
                best = state.u[e.to];
                next = e.to;
            }
        }

        if (next == -1)
            break;

        v = next;
    }

    return path;
}

// fa 3 cose:
// Marca nodi come coperti
//incrementa il flow degli archi del path 
//collega il source al primo nodo del path e l'ultimo nodo del path al sink, incrementando il flow di questi archi
static void markCovered(CoverState& state,
                        const std::vector<int>& path,
                        Graph& g) {

    if (path.empty()) return;

    int source = g.nodeIndex["global_source"];
    int sink   = g.nodeIndex["global_sink"];

    // source -> primo nodo
    state.getEdge(source, path[0]).flow++;

    for (int i = 0; i < path.size(); i++) {

        int v = path[i];
        state.covered[v] = 1;

        // archi del path
        if (i < path.size() - 1) {
            int u = path[i];
            int w = path[i+1];

            state.getEdge(u, w).flow++;
        }
    }

    // ultimo -> sink
    state.getEdge(path.back(), sink).flow++;
}
// controlla se tutti i nodi sono coperti e ritorna true se lo sono, false altrimenti
static bool allCovered(CoverState& state) {

    for (int x : state.covered)
        if (x == 0)
            return false;

    return true;
}

// imposta la demand a 1 per gli archi da Am a Ap, 0 altrimenti
static void initializeDemands(Graph& g, CoverState& state) {

    for (int u = 0; u < g.nodes.size(); u++) {

        for (auto& e : g.adj[u]) {

            std::string from = g.nodes[u].name;
            std::string to   = g.nodes[e.to].name;

            auto& edge = state.getEdge(u, e.to);

            if (!from.empty() && !to.empty() &&
                from.back() == 'm' && to.back() == 'p')
                edge.demand = 1;
            else
                edge.demand = 0;

            edge.flow = 0;
        }
    }
}

// questo metodo costruisce il grafo residuo e cerca un path da source a sink usando BFS,
//ritorna true se esiste un path da source a sink, false altrimenti.
static bool findAugmentingPath(Graph& g,
                              CoverState& state,
                              int s, //s è il nodo source, t è il nodo sink
                              int t,
                              std::vector<Parent>& parent) {
// costruisce il grafo residuo e cerca un path da source a sink usando BFS,
    std::queue<int> q;
    //vettore di booleani per tenere traccia dei nodi visitati
    std::vector<bool> visited(g.nodes.size(), false);
//inizializza la coda con il nodo source e marca source come visitato
    q.push(s);
    visited[s] = true;

    while (!q.empty()) {
        //prende un nodo u dalla coda
        int u = q.front(); q.pop();

        // ciclo sugli archi uscenti da u per considerare gli archi forward
        for (auto& e : g.adj[u]) {

            int v = e.to; //e.to è il nodo di destinazione dell'arco uscente da u 
            auto& edge = state.getEdge(u, v); //edge è lo stato dell'arco da u a v, che contiene la demand e il flow su quell'arco
            //se l'arco da u a v ha flow > demand e v non è stato visitato, marca v come visitato, imposta parent[v] a {u, true}

            // residuo forward
            if (edge.flow > edge.demand && !visited[v]) {

                visited[v] = true;
                parent[v] = {u, true};
                q.push(v);
            }
        }

        // ciclo sui predecessori di u per considerare anche gli archi backward
        for (int v : g.nodes[u].predecessors) {
        // residuo backward
            auto& edge = state.getEdge(v, u);
            //se l'arco da v a u ha flow > 0 e v non è stato visitato, marca v come visitato, 
            //imposta parent[v] a {u, false} e aggiungi v alla coda
            //in pratica l'arco da v a u è un arco backward se esiste un arco da v a u nel grafo originale e il flow su quell'arco è maggiore di 0, 
            //quindi possiamo "restituire" flow lungo quell'arco backward per cercare di trovare un path da source a sink nel grafo residuo
            if (edge.flow > 0 && !visited[v]) {

                visited[v] = true;
                parent[v] = {u, false};
                q.push(v);
            }
        }
    }
    //ritorna true se esiste un path da source a sink, false altrimenti
    return visited[t];
}
       

// decrementa il flow di 1 per ogni arco del path da s a t, seguendo le indicazioni in parent

static void augmentFlow(Graph& g,
                        CoverState& state,
                        int s,
                        int t,
                        std::vector<Parent>& parent) {
//partendo da t, risale il path fino a s, decrementando il flow di 1 per ogni arco del path
    int v = t;
//finché v non è uguale a s, prendi il nodo precedente u e verifica 
//se l'arco da u a v è un arco forward o backward. 
//Se è un arco forward, 
//decrementa il flow di 1. Se è un arco backward, incrementa il flow di 1. 
//Poi aggiorna v a u e continua il ciclo
    while (v != s) {

        int u = parent[v].prev;
        //se l'arco da u a v è un arco forward, decrementa il flow di 1. 
        //Se è un arco backward, incrementa il flow di 1.
        if (parent[v].forward)
            state.getEdge(u, v).flow--;
        else
            state.getEdge(v, u).flow++;

        v = u;
    }
}


//metodo che mi serve per la decomposizione in blocchi







};

#endif