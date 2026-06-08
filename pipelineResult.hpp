#ifndef PIPELINERESULT_H
#define PIPELINERESULT_H

#include "graph.hpp"
#include "blockDecompose.hpp"

#include <vector>

struct PipelineResult {

    Graph g;

    std::vector<int> topo;

    std::vector<std::vector<int>> MPC;

    std::vector<std::vector<int>> delta;

    std::vector<std::vector<int>> Z_all;

    std::vector<matchedBlock> matchedBlocks;
};

#endif