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





struct PipelineResult {

    Graph g;

    std::vector<int> topo;

    std::vector<std::vector<int>> MPC;

    std::vector<std::vector<int>> delta;

    std::vector<std::vector<int>> Z_all;

    std::vector<matchedBlock> matchedBlocks;
};



class wholeAlgoritmo {

public:




static void GFAFinale(const std::string& filename)
{
    PipelineResult result =
        runPipelineWithData(filename);

         // rimuove estensione .gfa
    std::string baseName =
        filename.substr(
            0,
            filename.find_last_of('.')
        );

    // nome output finale
    std::string outputName =
        baseName + "_output.gfa";

    if (filename == "E-3133.gfa")
    {
        writeGFACommentatoOld(
            filename,
            outputName,
            result.matchedBlocks,
            result.MPC,
            result.g
        );
    }
    else
    {
        writeGFACommentato(
            filename,
            outputName,
            result.matchedBlocks,
            result.MPC,
            result.g
        );
    }
}



// SCRITTURA GFA COMMENTATO


static void writeGFACommentatoOld(
    const std::string& inputFile,
    const std::string& outputFile,
    const std::vector<matchedBlock>& results,
    const std::vector<std::vector<int>>& MPC,
    const Graph& g
)
{
    std::ifstream in(inputFile);
    std::ofstream out(outputFile);

    if (!in || !out) {

        std::cerr << "Errore apertura file\n";

        return;
    }

    
    out << "# MPC\n";

    for (int i = 0; i < MPC.size(); i++) {

        out << "# p" << i << ": ";

        for (int node : MPC[i]) {

            out << g.nodes[node].name << " ";
        }

        out << "\n";
    }

    out << "\n";



    std::string line;

    int pathIndex = 0;

    while (std::getline(in, line)) {

        // commenta anche le righe P
        if (!line.empty() && line[0] == 'P') {

            out << "# " << line << "\n";

            if (pathIndex >= results.size()) {

                std::cerr
                    << "Mismatch tra P e results\n";

                continue;
            }

            const auto& mb =
                results[pathIndex++];

            // commento block decomposition
            out << "# ";

            for (const auto& b : mb.Blocks) {

                out << "p"
                    << b.path
                    << "x"
                    << b.matchedOnes
                    << "x"
                    << b.startIndex
                    << " ";
            }

            out << "\n";
        }
        else {

            // tutte le altre righe normali
            out << line << "\n";
        }
    }

    in.close();
    out.close();
}



static void writeGFACommentato(
    const std::string& inputFile,
    const std::string& outputFile,
    const std::vector<matchedBlock>& results,
    const std::vector<std::vector<int>>& MPC,
    const Graph& g
)
{
    std::ifstream in(inputFile);
    std::ofstream out(outputFile);

    if (!in || !out) {

        std::cerr << "Errore apertura file\n";

        return;
    }

// STAMPA MPC


out << "# MPC\n";

for (int i = 0; i < MPC.size(); i++) {

    out << "# W\tMPC\t"
        << i
        << "\tMPC\t1\t1\t";

    for (int node : MPC[i]) {

        std::string nodeName =
            g.nodes[node].name;

        out << ">"
            << nodeName;
    }

    out << "\n";
}

out << "\n";



    std::string line;

    int pathIndex = 0;

    while (std::getline(in, line)) {

        // commenta anche le righe W
        if (!line.empty() && line[0] == 'W') {

            out << "# " << line << "\n";

            if (pathIndex >= results.size()) {

                std::cerr
                    << "Mismatch tra W e results\n";

                continue;
            }

            const auto& mb =
                results[pathIndex++];

            out << "# ";

            for (const auto& b : mb.Blocks) {

                out << "p"
                    << b.path
                    << "x"
                    << b.matchedOnes
                    << "x"
                    << b.startIndex
                    << " ";
            }

            out << "\n";
        }
        else {

            // tutte le altre righe normali
            out << line << "\n";
        }
    }

    in.close();
    out.close();
}// VERSIONE COMPATTA (SOLO matchedBlocks)


static std::vector<matchedBlock>
runFullPipeline(const std::string& filename)
{
    PipelineResult result =
        runPipelineWithData(filename);

    return result.matchedBlocks;
}




// PIPELINE COMPLETA CON TUTTI I DATI


static PipelineResult
runPipelineWithData(const std::string& filename)
{
    PipelineResult result;

    // PARSING
GFAGraph parsed;
  if(filename=="E-3133.gfa")
    {
         parsed = readGFA(filename);
    }
    else
    {
         parsed = readGFA_W(filename);
    }
  

    Graph& g = parsed.g;

    std::vector<GFAPath>& gfaPaths =
        parsed.paths;



    if (g.nodes.empty()) {

        std::cerr << "Errore: grafo vuoto\n";

        return result;
    }

    if (!isDAG(g)) {

        std::cerr << "Errore: il grafo non è un DAG\n";

        return result;
    }


    result.g = g;



    auto topo = topologicalSort(g);

    result.topo = topo;


    auto MPC = computeMPC(g);

    result.MPC = MPC;


    std::vector<std::vector<int>> Z_all =
        convertGFAPathsToZ(
            g,
            gfaPaths,
            topo
        );

    result.Z_all = Z_all;


    auto delta =
        matriceBinaria(g,MPC,topo);

    result.delta = delta;


    // BLOCK DECOMPOSITION
    int num_paths = delta.size();

    BlockDecompose bd;

    for (const auto& z : Z_all) {

        matchedBlock mb =
            bd.BlocksDecompose(
                z,
                delta,
                num_paths
            );

        result.matchedBlocks.push_back(mb);
    }

    return result;
}



private:



static std::vector<std::vector<int>>
computeMPC(Graph& g)
{

    Graph gStar = convertGraph(g);



    CoverState state(gStar.nodes.size());



    auto initialPaths =
        Cover::computeInitialPathCover(
            gStar,
            state
        );



    int source =
        gStar.nodeIndex["global_source"];

    int sink =
        gStar.nodeIndex["global_sink"];



    Cover::reduceFlow(
        gStar,
        state,
        source,
        sink
    );



    auto finalPaths =
        Cover::extractFinalPaths(
            gStar,
            state,
            source,
            sink
        );


    auto cleanMPC =
        Cover::convertMPCtoOriginalGraph(
            gStar,
            g,
            finalPaths
        );


    return cleanMPC;
}

};

#endif