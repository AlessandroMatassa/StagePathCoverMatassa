#ifndef STATISTICS_H
#define STATISTICS_H

#include "metrics.hpp"

#include <fstream>
#include <iostream>

class Statistics {

public:

static void exportCSV(
    const std::string& inputGFA,
    const std::string& outputCSV
)
{
    PipelineResult data =wholeAlgoritmo::runPipelineWithData(inputGFA);

    GraphStatistics stats =Metrics::computeStatistics(data, data.fixedTinyBlockThreshold);

    std::vector<ZStatistics> zStats =Metrics::computeZStatistics(data, stats.tinyBlockCharThreshold);

    std::ofstream out(outputCSV);

if (!out.is_open()) {

    std::cerr << "Errore apertura CSV\n";

    return;
}




// GLOBAL STATS


out << "GLOBAL_STATS\n";

out << "numNodes;"
    << "numEdges;"
    << "numZ;"
    << "mpcSize;"
    << "averageZLength;"
    << "medianZLength;"
    << "averageBlocks;"
    << "medianBlocks;"
    << "averageBlockLength;"
    << "medianBlockLength;"
    << "averageBlockCharLength;"
    << "medianBlockCharLength;"

    << "maxBlockLength;"
    << "minBlockLength;"
    << "fragmentedPaths;"
    << "tinyBlocks;"
    << "tinyBlocksPercent;"
    << "tinyBlocksChar;"
    << "tinyBlocksCharPercent;"
    << "fragmentedPathsChar;"
    << "mostFragmentedPath;"
    << "averageOutdegree;"
    << "pathUsagePercent\n";

out << stats.numNodes << ";"
    << stats.numEdges << ";"
    << stats.numZ << ";"
    << stats.mpcSize << ";"
    << stats.averageZLength << ";"
    << stats.medianZLength << ";"
    << stats.averageBlocks << ";"
    << stats.medianBlocks << ";"
    << stats.averageBlockLength << ";"
    << stats.medianBlockLength << ";"

    << stats.averageBlockCharLength << ";"
    << stats.medianBlockCharLength << ";"
    << stats.maxBlockLength << ";"
    << stats.minBlockLength << ";"
    << stats.fragmentedPaths << ";"
    << stats.tinyBlocks << ";"
    << stats.tinyBlocksPercent << ";"
    << stats.tinyBlocksChar << ";"
    << stats.tinyBlocksCharPercent << ";"
    << stats.fragmentedPathsChar << ";"
    << stats.mostFragmentedPath << ";"
    << stats.averageOutdegree << ";"
    << stats.pathUsagePercent
    << "\n\n";



// ======================================================
// Z STATS
// ======================================================

out << "Z_STATS\n";

out << "zIndex;"
    << "zLength;"
    << "numBlocks;"
    << "averageBlockLength;"
    << "maxBlockLength;"
    << "minBlockLength;"
    << "tinyBlocks;"
    << "tinyBlocksChar;"
    << "tinyBlocksCharPercent;"
    << "fragmentedPathChar\n";

for (const auto& z : zStats) {

    out << z.zIndex << ";"
        << z.zLength << ";"
        << z.numBlocks << ";"
        << z.averageBlockLength << ";"
        << z.maxBlockLength << ";"
        << z.minBlockLength << ";"
        << z.tinyBlocks << ";"
        << z.tinyBlocksChar << ";"
        << z.tinyBlocksCharPercent << ";"
        << z.fragmentedPathChar
        << "\n";
}

out.close();

std::cout << "CSV salvato in "
          << outputCSV
          << "\n";
    // tutto il codice CSV identico
}

};

#endif