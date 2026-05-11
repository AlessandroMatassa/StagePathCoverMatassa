#ifndef WHOLEALGORITMO_H
#define WHOLEALGORITMO_H

#include "graph.hpp"
#include "cover.hpp"
#include "blockDecompose.hpp"

#include <vector>
#include <string>
#include <iostream>



#include <fstream>
#include <sstream>

class wholeAlgoritmo {
public:


static void GFAFinale(const std::string& filename)
{
    std::vector<matchedBlock> results = runFullPipeline(filename);
    writeGFACommentato(filename, "output.gfa", results);
}





static void writeGFACommentato(
    const std::string& inputFile,
    const std::string& outputFile,
    const std::vector<matchedBlock>& results
)
{
    std::ifstream in(inputFile);
    std::ofstream out(outputFile);

    if (!in || !out) {
        std::cerr << "Errore apertura file\n";
        return;
    }

    std::string line; 
    int pathIndex = 0;

    while (std::getline(in, line)) { // legge il file riga per riga

        
        out << line << "\n";  // scrive la riga originale nel file di output

        // controlla se è una riga P
        if (line[0] == 'P') { //!line.empty() && 


            if (pathIndex >= results.size()) { //superfluo?
                std::cerr << "Mismatch tra P e results\n";
                continue;
            }

            const auto& mb = results[pathIndex++];

            // costruisci commento
            out << "# ";

            for (const auto& b : mb.Blocks) {
                out << "p" << b.path
                    << "x" << b.length << " ";
            }

            out << "\n";
        }
    }

    in.close();
    out.close();
}


static std::vector<matchedBlock> runFullPipeline(const std::string& filename)
{
    //  PARSING

    GFAGraph parsed = readGFA(filename);
    Graph& g = parsed.g;
    std::vector<GFAPath>& gfaPaths = parsed.paths;

   
    if (g.nodes.empty()) {
        std::cerr << "Errore: grafo vuoto\n";
        return {};
    }

    if (!isDAG(g)) { 
        std::cerr << "Errore: il grafo non è un DAG\n";
        return {};
    }

    // cose varie
   
    auto topo = topologicalSort(g);
    
    auto MPC = computeMPC(g);

  
    // COSTRUZIONE Z
    std::vector<std::vector<int>> Z_all =
        convertGFAPathsToZ(g, gfaPaths, topo);

    // BLOCK DECOMPOSITION

    std::vector<matchedBlock> results;

     auto delta = matriceBinaria(g, MPC, topo);

    int num_paths = delta.size();

    // BLOCK DECOMPOSE
    BlockDecompose bd;

    for (const auto& z : Z_all) {

        matchedBlock mb = bd.BlocksDecompose(
            z,
            delta,
            num_paths
        );

        results.push_back(mb);
    }

    // OUTPUT
    return results;
}
private:
static std::vector<std::vector<int>> computeMPC(Graph& g)
{
   
    //  COSTRUZIONE G*
    Graph gStar = convertGraph(g);

   
    CoverState state(gStar.nodes.size());

    auto initialPaths = Cover::computeInitialPathCover(gStar, state);
   
    int source = gStar.nodeIndex["global_source"];
    int sink   = gStar.nodeIndex["global_sink"];

    Cover::reduceFlow(gStar, state, source, sink);

    
    //ESTRAZIONE PATH FINALI
    
    auto finalPaths = Cover::extractFinalPaths(gStar, state, source, sink);

   
    auto cleanMPC = Cover::convertMPCtoOriginalGraph(
        gStar,
        g,
        finalPaths
    );

    return cleanMPC;
}


};
#endif