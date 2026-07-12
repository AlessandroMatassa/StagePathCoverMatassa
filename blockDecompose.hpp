#ifndef BLOCK_DECOMPOSE_H
#define BLOCK_DECOMPOSE_H
#include "graph.hpp"
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <list>


struct  longestMatchResult {
    int path; //il numero del path che viene seguito in un dato blocco
    int length; //la lunghezza del path più lungo trovato
    int matchedOnes; //il numero di 1 che sono stati matchati in questo blocco
    int startIndex; //l'indice di z da cui inizia il blocco

    int charLength; //la lunghezza in caratteri del blocco, calcolata come la somma delle lunghezze dei nodi corrispondenti ai 1 matchati in questo blocco
    //////
    int zStart;
    int zEnd;
    int endIndex;
    //////
};
struct matchedBlock {
    std::vector<int> path; //lista di interi che ha n volte 
                          //il numero del path che viene seguito 
                          //in un dato blocco
                          //es: 111 3333 222 4 111 (1 blocco path 1 x3 2 blocco
                          //path 3 x4 3 blocco path 2 x3 4 blocco path 1 x3)
                          
    
  std::list<longestMatchResult> Blocks; //la lista di longestMatchResult che contiene i risultati di longestMatch per ogni blocco
};


class BlockDecompose 
{
    public:      //static std::list< std::vector<int>> BlocksDecompose(const std::vector<int>& z, const std::vector<std::vector<int>>& delta, int num_rows, int num_cols){
   

    matchedBlock BlocksDecompose(const std::vector<int>& z, const std::vector<std::vector<int>>& delta, int num_rows)
    {
        int index = 0;
        int n = delta[0].size(); //n numero colonne di delta --> numeri di nodi
        std::vector<int> currentNodeIndex(num_rows, 0);
        matchedBlock matchedBlockList;
            while (index < n) 
            {
                while (index < n && z[index] == 0) //finché z[index] è 0, aggiungo -1 alla lista matchedBlockList e incremento index
                {                               //significa che metto -1 perché non c'è nessun path attivo per quel nodo, e passo al nodo successivo
                    matchedBlockList.path.push_back(-1);
                    index++;
                }
                if(index == n) //quando esco dal while, se index è uguale a n, significa che ho finito di scorrere tutte le colonne di delta, quindi ritorno la lista matchedBlockList
                    return matchedBlockList; 
                //altrimenti, se index è minore di n, significa che ho trovato un blocco di colonne consecutive in cui z[index] è 1, quindi chiamo longestMatch per trovare il path più lungo che posso seguire a partire da index
                longestMatchResult result = longestMatch(z, delta, index); // auto [p, l] = longestMatch(z, delta, index)
                int p = result.path;
                int l = result.length;
                if (result.path != -1) {result.startIndex =currentNodeIndex[result.path];}
                /*  
                
                                                                while z[index + l − 1] = 0 do //p è il path restituito da longest match
                                                15:                   l ← l − 1 //l è la lunghezza del match più lunga restituita dal longest match
                                                16:                  end while 
                                                17:                  index ← index + l
                                                18:               Aggiungi p ripetuto l volte a matched block
                */
                        int end = index + l - 1;

                        // cerca ultimo 1 nel blocco
                        while (end >= index && z[end] == 0) {
                            end--;
                        }
                        // nuova lunghezza
                        l = (end >= index) ? (end - index + 1) : 0;
                        result.length = l;

                       result.zStart = index;
                        result.zEnd   = index + l - 1;


                index += l; //aggiorno index spostandomi alla fine del blocco che ho appena trovato, in modo da cercare il prossimo blocco a partire da index
                for (int i = 0; i < l; i++)
                {    //aggiungo p ripetuto l volte a matched block
                    matchedBlockList.path.push_back(p);
                }
                if (result.path != -1) {currentNodeIndex[result.path]+= result.matchedOnes;}
                
                matchedBlockList.Blocks.push_back(result);
                
            }
        return matchedBlockList;
    };
longestMatchResult longestMatch(const std::vector<int>& z, const std::vector<std::vector<int>>& delta, int index)
{  
    longestMatchResult result;

    int best_path = -1;
    int best_len = 0;

    int best_matched_ones = 0; //new
    

    int num_paths = delta.size(); 
    int n = z.size();
    
    for (int p = 0; p < num_paths; p++) {

        int len = 0;
                int matchedOnes = 0;


                                                /*  while (index + len < n &&
                                                        delta[p][index + len] == z[index + len]) */ 
                            
    // finché non arrivo alla fine di z e delta[p] è uguale a z per tutte le colonne
    // del blocco che sto considerando incremento len                                                     
           while (index + len < n &&
       delta[p][index + len] == z[index + len]){

        if (
                z[index + len] == 1 &&
                delta[p][index + len] == 1
            ) 
            {
                matchedOnes++;
            }

            len++;
        }

        if (len > best_len) {
            best_len = len;
            best_path = p;
            best_matched_ones = matchedOnes;
            

        }
    }

    result.path = best_path;
    result.length = best_len;
    result.matchedOnes = best_matched_ones;
    return result;
}


matchedBlock BlocksDecomposeOptimized(
    const std::vector<int>& z,
    const std::vector<std::vector<int>>& delta,
    int num_rows,
    const Graph& g,
    const std::vector<int>& topo
)
{
    const int TINY_THRESHOLD = 38513/2; //ho impostato sto numero per matchare il risultato della

    auto nodeLength = [&](int topoPos) 
    {
        int realNode = topo[topoPos];
        return g.nodes[realNode].sequenceLength;
    };

    int index = 0;
    int n = delta[0].size();

    std::vector<int> currentNodeIndex(num_rows, 0);

    matchedBlock matchedBlockList;

    while (index < n)
    {
        while (index < n && z[index] == 0)
        {
            matchedBlockList.path.push_back(-1);
            index++;
        }

        if (index == n)
        {
            return matchedBlockList;
        }

        longestMatchResult result = longestMatch(z, delta, index);

        int p = result.path;
        int l = result.length;

        if (p != -1)
        {
            result.startIndex = currentNodeIndex[p];
        }

        int end = index + l - 1;

        while (end >= index && z[end] == 0)
        {
            end--;
        }

        l = (end >= index) ? (end - index + 1) : 0;

        result.length = l;

        result.zStart = index;
        result.zEnd = end;

        result.endIndex = result.startIndex + result.matchedOnes - 1;

        int chars = 0;

        for (int i = result.zStart; i <= result.zEnd; i++)
        {
            if (z[i] == 1)
            {
                chars += nodeLength(i);
            }
        }

        result.charLength = chars;

        
        // REBALANCE TINY BLOCK
        

        if (result.charLength < TINY_THRESHOLD &&
            !matchedBlockList.Blocks.empty())
        {
            auto prevIt = std::prev(matchedBlockList.Blocks.end());

            longestMatchResult& prev = *prevIt;

            int gainChars = 0;
            int gainOnes = 0;

            bool found = false;

            int bestBoundary = result.zStart;
            int bestStartOne = result.zStart;

            int bestGainChars = 0;
            int bestGainOnes = 0;

            int pos = result.zStart - 1;

            while (pos >= prev.zStart)
            {
                if (delta[result.path][pos] != z[pos])
                {
                    break;
                }

                if (z[pos] == 1)
                {
                    gainChars += nodeLength(pos);
                    gainOnes++;

                    bestStartOne = pos;
                }

                int futureCurr = result.charLength + gainChars;
                int futurePrev = prev.charLength - gainChars;

                if (futureCurr >= TINY_THRESHOLD &&
                    futurePrev >= TINY_THRESHOLD)
                {
                    found = true;

                    bestBoundary = bestStartOne;

                    bestGainChars = gainChars;
                    bestGainOnes = gainOnes;
                }
                    
                pos--;
            }

            auto countOnes = [&](int start, int end)
            {
                int cnt = 0;

                for (int i = start; i <= end; i++)
                {
                    if (z[i] == 1)
                    {
                        cnt++;
                    }
                }

                return cnt;
            };

            if (found)
            {
                result.zStart = bestBoundary;

                prev.zEnd = bestBoundary - 1;

                // trim finale identico
                // alla BlockDecompose

                while (prev.zEnd >= prev.zStart &&
                       z[prev.zEnd] == 0)
                {
                    prev.zEnd--;
                }

                prev.length = prev.zEnd - prev.zStart + 1;
                result.length = result.zEnd - result.zStart + 1;

                prev.matchedOnes = countOnes(prev.zStart, prev.zEnd);
                result.matchedOnes = countOnes(result.zStart, result.zEnd);

                prev.charLength -= bestGainChars;
                result.charLength += bestGainChars;

                //   prev.matchedOnes -= bestGainOnes;
                //   result.matchedOnes += bestGainOnes;

                //   prev.endIndex -= bestGainOnes;

                //   result.startIndex -= bestGainOnes;
                //   result.endIndex = result.startIndex + result.matchedOnes - 1;
            }
        }

        index += l;

        for (int i = 0; i < l; i++)
        {
            matchedBlockList.path.push_back(p);
        }

        if (p != -1)
        {
            currentNodeIndex[p] += result.matchedOnes;
        }

        matchedBlockList.Blocks.push_back(result);
    }

    return matchedBlockList;
}












//ora i metodi per fare inverso

int contaNodiNelPath(const std::vector<int>& row)
{
    int count = 0;

    for (int x : row)
        if (x == 1)
            count++;

    return count;
}



    matchedBlock BlocksDecomposeBackward(const std::vector<int>& z, const std::vector<std::vector<int>>& delta, int num_rows)
    {
        int index = z.size() - 1;
        int n = delta[0].size(); //n numero colonne di delta --> numeri di nodi

       
        std::vector<int> currentNodeIndex(num_rows);

        for (int p = 0; p < num_rows; p++) {
            currentNodeIndex[p] =
                contaNodiNelPath(delta[p]) - 1;
        }

        matchedBlock matchedBlockList;
            while (index >= 0)
            {
                while (index >= 0 && z[index] == 0)
                {                               //significa che metto -1 perché non c'è nessun path attivo per quel nodo, e passo al nodo successivo
                    matchedBlockList.path.push_back(-1);
                    index--;
                }
                if (index < 0)
                    break;
    
                //altrimenti, se index è minore di n, significa che ho trovato un blocco di colonne consecutive in cui z[index] è 1, quindi chiamo longestMatch per trovare il path più lungo che posso seguire a partire da index
                longestMatchResult result = longestMatchBackward(z, delta, index); // auto [p, l] = longestMatch(z, delta, index)
                int p = result.path;
                int l = result.length;
                if (result.path != -1) {
                    result.startIndex =currentNodeIndex[result.path] - result.matchedOnes + 1;
                    result.endIndex =currentNodeIndex[result.path];
                }
                /*  
                
                                                                while z[index + l − 1] = 0 do //p è il path restituito da longest match
                                                15:                   l ← l − 1 //l è la lunghezza del match più lunga restituita dal longest match
                                                16:                  end while 
                                                17:                  index ← index + l
                                                18:               Aggiungi p ripetuto l volte a matched block
                */
                 int start = index - l + 1;

                    while (start <= index && z[start] == 0) {
                        start++;
                    }

                    l = (start <= index)
                        ? (index - start + 1)
                        : 0;

                    result.length = l;

                    

                    ////////
                    result.zStart = start;
                    result.zEnd   = index;
                    ////////
                l = (start <= index)
                ? (index - start + 1)
                : 0;

            result.length = l;

            result.zStart = start;
            result.zEnd   = index;
               index -= l; //aggiorno index spostandomi alla fine del blocco che ho appena trovato, in modo da cercare il prossimo blocco a partire da index
                for (int i = 0; i < l; i++)
                {    //aggiungo p ripetuto l volte a matched block
                    matchedBlockList.path.push_back(p);
                }
                if (result.path != -1) {currentNodeIndex[result.path]-= result.matchedOnes;}
                
                matchedBlockList.Blocks.push_back(result);
                
            }
            // inverto la rappresentazione costruita da destra verso sinistra
        std::reverse( matchedBlockList.path.begin(), matchedBlockList.path.end());

        matchedBlockList.Blocks.reverse();
        return matchedBlockList;
    };

    longestMatchResult longestMatchBackward(const std::vector<int>& z, const std::vector<std::vector<int>>& delta, int index)
{  
    longestMatchResult result;

    int best_path = -1;
    int best_len = 0;

    int best_matched_ones = 0; //new
    

    int num_paths = delta.size(); 
    int n = z.size();
    
    for (int p = 0; p < num_paths; p++) {

        int len = 0;
                int matchedOnes = 0;


                                                /*  while (index + len < n &&
                                                        delta[p][index + len] == z[index + len]) */ 
                            
    // finché non arrivo alla fine di z e delta[p] è uguale a z per tutte le colonne
    // del blocco che sto considerando incremento len                                                     
           while (index - len >= 0 && delta[p][index - len] == z[index - len]){

        if (z[index - len] == 1 && delta[p][index - len] == 1) 
            {
                matchedOnes++;
            }

            len++;
        }

        if (len > best_len) {
            best_len = len;
            best_path = p;
            best_matched_ones = matchedOnes;
            

        }
    }

    result.path = best_path;
    result.length = best_len;
    result.matchedOnes = best_matched_ones;
    return result;
}

};

//il grafo dev'essere gstar e topologico, assumere che sia così o aggiungere un check?
std::vector<std::vector<int>> matriceBinaria(Graph& g, const std::vector<std::vector<int>>& MPC , std::vector<int> topo)
{
    // auto topo = topologicalSort(g);

    int num_paths = MPC.size();
    int num_nodes = topo.size();

    // vettore che mappa l'indice del nodo alla sua posizione nella topological sort
    std::vector<int> topoIndex(num_nodes);

    for (int i = 0; i < num_nodes; i++) {
        topoIndex[topo[i]] = i;
    }
    //matrice inizializzata a 0
    std::vector<std::vector<int>> delta(
        num_paths,
        std::vector<int>(num_nodes, 0)
    );

    for (int i = 0; i < num_paths; i++) {
        for (int nodeIndex : MPC[i]) { 

            int col = topoIndex[nodeIndex];
            delta[i][col] = 1;
        }
    }

    return delta;
}





#endif