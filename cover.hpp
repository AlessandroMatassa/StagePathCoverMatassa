#ifndef COVER_H
#define COVER_H

#include "graph.hpp"
#include <vector>
#include <algorithm>

struct CoverState {
  //  std::vector<int> m; 
    std::vector<int> u; // valore percorso dal nodo all'indice 
    std::vector<int> covered; //indica se un nodo è coperto o no

    CoverState(int n) {
   //     m.resize(n, 1);
        u.resize(n, 0);
        covered.resize(n, 0);
    }
};

class Cover {
public:

    static std::vector<std::vector<int>> computeInitialPathCover(Graph& g) {

        std::vector<std::vector<int>> paths;
        std::vector<int> topo = topologicalSort(g);
        CoverState state(g.nodes.size());

        while (true) { //si ferma quando sono stati coperti tutti i nodi sono coperti o quando u max è zero

            computeU(g, topo, state); //calcola u per ogni nodo, partendo dall'ordinamento topologico e dallo stato della copertura

            int start = findBestStart(state);

            if (start == -1 || state.u[start] == 0)
                break;

            std::vector<int> path = extractPath(g, state, start);

            markCovered(state, path); //segna i nodi coperti dal percorso appena estratto, dato lo stato della copertura e il percorso appena estratto

            paths.push_back(path); //aggiunge il percorso appena estratto alla lista dei percorsi

            if (allCovered(state)) 
                break;
        }

        return paths;
    }

private:
 //algoritmo per calcolare u, partendo dall'ordinamento topologico e dallo stato della copertura
 // u sarebbe il numero di nodi coperti da un percorso che parte da v, se v non è coperto, altrimenti 0
    static void computeU(Graph& g,
                         const std::vector<int>& topo,
                         CoverState& state) {

        for (auto it = topo.rbegin(); it != topo.rend(); ++it) { 
 
            int v = *it; 

            int best = 0; //best è il massimo contributo dei successori di v, inizialmente 0

            // Scorre tutti i successori di v (archi in g.adj[v]) e usa state.u[e.to] per tenere il massimo contributo raggiungibile.
            for (auto& e : g.adj[v]) {
                best = std::max(best, state.u[e.to]); //prende il massimo tra best e u del nodo di destinazione dell'arco e
            }

            int val = (state.covered[v] == 0 ? 1 : 0); // se v non è coperto, val è 1, altrimenti è 0

            state.u[v] = val + best; // u di v è 1 (se v non è coperto) più il massimo contributo dei suoi successori
        }
    }
//algoritmo per trovare il nodo da cui partire, ovvero quello con u più alto
// se tutti i nodi sono coperti, o se il massimo è 0, allora ritorna -1
// se invece c'è un nodo con u > 0, allora ritorna il nodo con u più alto
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
//ritorna il percorso che parte da start e segue sempre l'arco con u più alto, 
//fino a quando non si arriva a un nodo senza archi uscenti o con u = 0
    static std::vector<int> extractPath(Graph& g,
                                         CoverState& state,
                                         int start) {

        std::vector<int> path;
        int v = start;

        while (true) {

            path.push_back(v); // aggiungo v al percorso

            int best = -1;
            int next = -1;

            for (auto& e : g.adj[v]) { // per ogni arco uscente da v, 
                                //se u del nodo di destinazione è più alto del best, aggiorna best e next

                if (state.u[e.to] > best) {
                    best = state.u[e.to];
                    next = e.to;
                }
            }

            if (next == -1) //caso in cui non ci sono archi uscenti
                break;

            v = next;
        }

        return path;
    }
 // algoritmo per segnare i nodi coperti da un percorso, dato lo stato della copertura e il percorso appena estratto
    static void markCovered(CoverState& state,
                           const std::vector<int>& path) {

        for (int v : path) {
            state.covered[v] = 1;
        }
    }
 // algoritmo per controllare se tutti i nodi sono coperti, dato lo stato della copertura
    static bool allCovered(CoverState& state) { //prende in input lo stato della copertura

        for (int x : state.covered) // se c'è un nodo non coperto, ritorna false
            if (x == 0)
                return false;

        return true;
    }
};

#endif