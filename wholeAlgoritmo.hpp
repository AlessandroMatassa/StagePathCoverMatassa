#ifndef WHOLEALGORITMO_H
#define WHOLEALGORITMO_H

#include "graph.hpp"
#include "cover.hpp"
#include "blockDecompose.hpp"
#include "metrics.hpp"
#include "pipelineResult.hpp"
#include "BlockOptimizer.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>






/* struct PipelineResult {

    Graph g;

    std::vector<int> topo;

    std::vector<std::vector<int>> MPC;

    std::vector<std::vector<int>> delta;

    std::vector<std::vector<int>> Z_all;

    std::vector<matchedBlock> matchedBlocks;
}; */



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


    auto MPC = computeMPC(g); //la struttura di MPC è 

    result.MPC = MPC;


    std::vector<std::vector<int>> Z_all =convertGFAPathsToZ(g,gfaPaths,topo);

    result.Z_all = Z_all;


    auto delta =matriceBinaria(g,MPC,topo);

    result.delta = delta;


    //posso far diventare questa cosa un ciclo   
    // BLOCK DECOMPOSITION INIZIALE
    

    int num_paths = delta.size();

    BlockDecompose bd;

    for (const auto& z : Z_all) {
        matchedBlock mb =
           bd.BlocksDecompose(z,delta,num_paths);
           //bd.BlocksDecomposeBackward(z, delta, num_paths);
          //bd.BlocksDecomposeOptimized(z, delta, num_paths, g, topo); //attivare per optimized
            //blockoptimizer
            // mb =BlockOptimizer::optimizeTinyBlocks(mb,z,topo,delta,g);
        result.matchedBlocks.push_back(mb);
    }



    
    // CALCOLO METRICS
    
    double fixedThreshold = -1;
//ciclo per aggiungere X path ogni volta alla MPC

for(int i = 0; i < 3; i++) //ora dovrebbe aggiungere alla MPC i primi 3 path più frammentati,
{
    
    GraphStatistics stats = Metrics::computeStatistics(result, fixedThreshold);
    if(fixedThreshold == -1) // se è la prima iterazione, setta la soglia per i tiny block in base alla statistica calcolata sui blocchi iniziali
    { 
        fixedThreshold = stats.tinyBlockCharThreshold; 
        result.fixedTinyBlockThreshold =fixedThreshold;
    } 
    //GreedyPath 
      if(i !=3) // aggiunge un greedy path alla MPC solo nelle prime due iterazioni, poi esce dal ciclo di refinement
      {
        std::vector<int> greedyPath =Cover:: buildGreedyTinyPath(result.g, result.topo, stats.tinyNodeScores);
        //std::vector<int> greedyPath2 =Cover:: buildGreedyTinyPath(result.g, result.topo, stats.relativeTinyNodeScores);

          result.MPC.push_back(greedyPath);

          // RICOMPUTA BLOCK DECOMPOSITION
        
        result.delta =matriceBinaria(result.g,result.MPC,result.topo);
        result.matchedBlocks.clear();

        int refined_num_paths =result.delta.size(); 

        for (const auto& z : result.Z_all)
        {
            matchedBlock mb =
               bd.BlocksDecompose(z,result.delta,refined_num_paths);
               //bd.BlocksDecomposeBackward(z, result.delta, refined_num_paths);
                //blockoptimizer
             
              //  bd.BlocksDecomposeOptimized(z, result.delta, refined_num_paths, result.g, result.topo);
            result.matchedBlocks.push_back(mb);
        }
        //col ciclo che termina qui
        stats=Metrics::computeStatistics(result, fixedThreshold);
  }
    
//per i giri di sola greedy path poi rimuovere

    
    // REFINEMENT MPC classico
    if(i==3)
    {
        //     se c'è una Z frammentata, prendi la Z più frammentata, convertila in path reale, aggiungi questo path alla MPC, ricostruisci delta e ricalcola la block decomposition per tutte le Z, altrimenti esci dal ciclo di refinement
    if (stats.mostFragmentedPath != -1)
    {
        
        
            // prende la Z più frammentata
            const auto& fragmentedZ =result.Z_all[stats.mostFragmentedPath];
            // converte Z in path reale
            std::vector<int> newPath =convertZtoPath(fragmentedZ,result.topo);

            // aggiunge nuovo path alla MPC
            result.MPC.push_back(newPath);
        
            
        // RICOSTRUZIONE DELTA    
        result.delta =matriceBinaria(result.g,result.MPC,result.topo);
        
        // RICOMPUTA BLOCK DECOMPOSITION
        

        result.matchedBlocks.clear();

        int refined_num_paths =result.delta.size(); 

        for (const auto& z : result.Z_all)
        {
            matchedBlock mb =
               bd.BlocksDecompose(z,result.delta,refined_num_paths);
               //bd.BlocksDecomposeBackward(z, result.delta, refined_num_paths);
                //blockoptimizer
             
              //  bd.BlocksDecomposeOptimized(z, result.delta, refined_num_paths, result.g, result.topo);
            result.matchedBlocks.push_back(mb);
        }
        //col ciclo che termina qui
    }
    else
        break; // se non c'è una Z frammentata, esce dal ciclo di refinement
        
}   
}
    return result;
}



private:
static std::vector<int>
convertZtoPath(const std::vector<int>& z, const std::vector<int>& topo)
{
    std::vector<int> path;

    for (int i = 0; i < z.size(); i++) {

        if (z[i] == 1) {

            path.push_back(
                topo[i]
            );
        }
    }

    return path;
}



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



    auto finalPaths =Cover::extractFinalPaths(gStar,state,source,sink);


    auto cleanMPC =Cover::convertMPCtoOriginalGraph(gStar,g,finalPaths);


    return cleanMPC;
}

};

#endif