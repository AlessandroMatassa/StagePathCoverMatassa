    #ifndef METRICS_H
    #define METRICS_H

    #include "pipelineResult.hpp"

    #include <vector>
    #include <algorithm>
    #include <numeric>
    #include <set>
    #include <limits>

    struct GraphStatistics {


        double averageBlockCharLength = 0.0;
        double medianBlockCharLength = 0.0;
        double tinyBlockCharThreshold = 0.0;
       

        int numNodes = 0;
        int numEdges = 0;

        int numZ = 0;

        int mpcSize = 0;

        double averageZLength = 0.0;
        double medianZLength = 0.0;

        double averageBlocks = 0.0;
        double medianBlocks = 0.0;

        double averageBlockLength = 0.0;
        double medianBlockLength = 0.0;

        int maxBlockLength = 0;
        int minBlockLength = 0;

        int fragmentedPaths = 0;

        int tinyBlocks = 0;
        double tinyBlocksPercent = 0.0;


        // NUOVE STATISTICHE
        

        int tinyBlocksChar = 0;

        double tinyBlocksCharPercent = 0.0;

        int fragmentedPathsChar = 0;

        int mostFragmentedPath = -1;

        double averageOutdegree = 0.0;

        double pathUsagePercent = 0.0;


        std::vector<int> tinyNodeScores;
        std::vector<int> relativeTinyNodeScores;
    };



    struct ZStatistics {

        int zIndex = 0;

        int zLength = 0;

        int numBlocks = 0;

        double averageBlockLength = 0.0;

        int maxBlockLength = 0;

        int minBlockLength = 0;

        int tinyBlocks = 0;

        int tinyBlocksChar = 0;

        double tinyBlocksCharPercent = 0.0;

        bool fragmentedPathChar = false;
    };



    class Metrics {

    public:




    // Z STATISTICS


    static std::vector<ZStatistics>
    computeZStatistics(const PipelineResult& data ,double tinyThreshold

    )
    {
        std::vector<ZStatistics> results;

        const int TINY_BLOCK_THRESHOLD = 2;

       // const int TINY_BLOCK_CHAR_THRESHOLD =   35;

        const int TINY_BLOCK_CHAR_THRESHOLD =(int)tinyThreshold;

        const int FRAGMENT_CHAR_THRESHOLD = 5;

        for (int i = 0; i < data.Z_all.size(); i++) {

            ZStatistics zstats;

            zstats.zIndex = i;

            const auto& z = data.Z_all[i];

            const auto& mb = data.matchedBlocks[i];

            int zLength = 0;

            for (int x : z) {
                zLength += x;
            }

            zstats.zLength = zLength;

            zstats.numBlocks =
                mb.Blocks.size();

            int totalBlockLength = 0;

            int topoPos = 0;

            int tinyCharCounter = 0;

            zstats.minBlockLength =
                std::numeric_limits<int>::max();

            for (const auto& b : mb.Blocks) {

                totalBlockLength += b.matchedOnes;

                if (b.matchedOnes > zstats.maxBlockLength) {
                    zstats.maxBlockLength = b.matchedOnes;
                }

                if (b.matchedOnes < zstats.minBlockLength) {
                    zstats.minBlockLength = b.matchedOnes;
                }

                if (b.matchedOnes <= TINY_BLOCK_THRESHOLD) {
                    zstats.tinyBlocks++;
                }

                        int charLength = 0;

                int matchedNodesSeen = 0;

                int topoPos = b.zStart;

                while (
                    topoPos < z.size()
                    &&
                    matchedNodesSeen < b.matchedOnes
                )
                {
                    if (z[topoPos] == 1)
                    {
                        int realNode =
                            data.topo[topoPos];

                        charLength +=
                            data.g.nodes[realNode]
                            .sequenceLength;

                        matchedNodesSeen++;
                    }

                    topoPos++;
            }

                if (charLength < TINY_BLOCK_CHAR_THRESHOLD) {

                    zstats.tinyBlocksChar++;

                    tinyCharCounter++;
                }
            }

            if (!mb.Blocks.empty()) {

                zstats.tinyBlocksCharPercent =(100.0 *zstats.tinyBlocksChar)/mb.Blocks.size();
            }

            if (tinyCharCounter > FRAGMENT_CHAR_THRESHOLD) {

                zstats.fragmentedPathChar = true;
            }

            if (!mb.Blocks.empty()) {

                zstats.averageBlockLength =(double) totalBlockLength
                    /
                    mb.Blocks.size();
            }

            if (mb.Blocks.empty()) {

                zstats.minBlockLength = 0;
            }

            results.push_back(zstats);
        }

        return results;
    }




    // GLOBAL STATISTICS


    static GraphStatistics
    computeStatistics(const PipelineResult& data, double FixedtinyThreshold
    )
    {
        GraphStatistics stats;

        stats.tinyNodeScores.assign(data.topo.size(),0);


        stats.numNodes =data.g.nodes.size();

        int edgeCount = 0;
            std::vector<int> blockCharLengths;

         /*   struct BlockCharInfo
            {
                int zIndex;
                int charLength;
            };  */

            struct BlockCharInfo
            {
                int zIndex;
                int charLength;

                int zStart;
                int zEnd;
            };



            std::vector<BlockCharInfo> blockInfos;


        for (const auto& adj : data.g.adj) {
            edgeCount += adj.size();
        }

        stats.numEdges = edgeCount;

        stats.numZ =
            data.Z_all.size();

        stats.mpcSize =
            data.MPC.size();




        // Z LENGTHS
        

        std::vector<int> zLengths;

        for (const auto& z : data.Z_all) {

            int len = 0;

            for (int x : z) {
                len += x;
            }

            zLengths.push_back(len);
        }

        if (!zLengths.empty()) {

            double sum =
                std::accumulate(
                    zLengths.begin(),
                    zLengths.end(),
                    0.0
                );

            stats.averageZLength =
                sum / zLengths.size();
        }

        if (!zLengths.empty()) {

            std::sort(zLengths.begin(),zLengths.end());

            int middle = zLengths.size() / 2;

            if (zLengths.size() % 2 == 0) {

                stats.medianZLength =(zLengths[middle - 1]+zLengths[middle]) / 2.0;
            }
            else 
            {
                stats.medianZLength =zLengths[middle];
            }
        }



        
        // BLOCK COUNTS
        

        std::vector<int> blockCounts;

        for (const auto& mb : data.matchedBlocks) {

            int blocks =
                mb.Blocks.size();

            blockCounts.push_back(blocks);
        }

        if (!blockCounts.empty()) {

            double sum =
                std::accumulate(
                    blockCounts.begin(),
                    blockCounts.end(),
                    0.0
                );

            stats.averageBlocks =
                sum / blockCounts.size();
        }

        if (!blockCounts.empty()) {

            std::sort(
                blockCounts.begin(),
                blockCounts.end()
            );

            int middle =
                blockCounts.size() / 2;

            if (blockCounts.size() % 2 == 0) {

                stats.medianBlocks =
                    (blockCounts[middle - 1]+blockCounts[middle]) / 2.0;
            }
            else {

                stats.medianBlocks =blockCounts[middle];
            }
        }



        
        // BLOCK ANALYSIS
        

        std::vector<int> blockLengths;

        const int TINY_BLOCK_THRESHOLD = 2;

    // const int TINY_BLOCK_CHAR_THRESHOLD = //35;
        //   stats.medianBlockCharLength / 2;

        const int FRAGMENT_THRESHOLD = 10;

        const int FRAGMENT_CHAR_THRESHOLD = 5;

        int tinyBlocksCounter = 0;

    // int tinyBlocksCharCounter = 0;

        //int fragmentedCharCounter = 0;

    // int maxTinyCharBlocks = -1;

    // int mostFragmentedPathIndex = -1;



        for (int zIndex = 0;zIndex < data.matchedBlocks.size();zIndex++)
        {
            const auto& mb =
                data.matchedBlocks[zIndex];

            const auto& z =
                data.Z_all[zIndex];

            int topoPos = 0;

        // int tinyCharInsideThisZ = 0;

            for (const auto& b : mb.Blocks)
            {

                blockLengths.push_back(b.matchedOnes);
                
                // tiny block classico
                

                if (b.matchedOnes <= TINY_BLOCK_THRESHOLD)
                {
                    tinyBlocksCounter++;
                }




                // tiny block biologico


                        int charLength = 0;

                int matchedNodesSeen = 0;

                int topoPos = b.zStart;

                while (topoPos < z.size() && matchedNodesSeen < b.matchedOnes)
                {
                    if (z[topoPos] == 1)
                    {
                        int realNode =
                            data.topo[topoPos];

                        charLength +=
                            data.g.nodes[realNode].sequenceLength;
                            //.name.size();

                        matchedNodesSeen++;
                    }

                    topoPos++;
                }

                blockCharLengths.push_back(charLength);

                //blockInfos.push_back({zIndex,charLength});

                blockInfos.push_back({zIndex,charLength,b.zStart,b.zEnd});
            }


           
       
     }
     stats.tinyBlocks = tinyBlocksCounter;

        if (!blockLengths.empty())
        {
            stats.tinyBlocksPercent =
                (
                    100.0 *
                    tinyBlocksCounter
                )
                /
                blockLengths.size();
        }


            // fragmented biologico
        /* 

            if (tinyCharInsideThisZ>FRAGMENT_CHAR_THRESHOLD)
            {
                fragmentedCharCounter++;
            }

    */

            
            /* most fragmented path
            

            if (tinyCharInsideThisZ > maxTinyCharBlocks && tinyCharInsideThisZ > FRAGMENT_CHAR_THRESHOLD)
            {
                maxTinyCharBlocks =tinyCharInsideThisZ;

                mostFragmentedPathIndex =zIndex;
            }
        }



        stats.tinyBlocks =tinyBlocksCounter;

        stats.tinyBlocksChar =tinyBlocksCharCounter;

        stats.fragmentedPathsChar = fragmentedCharCounter;

        stats.mostFragmentedPath = mostFragmentedPathIndex;

    */

        
        // PERCENTUALI
    


        
        // BLOCK LENGTH STATS
        

        if (!blockLengths.empty()) {

            double sum =std::accumulate(blockLengths.begin(),blockLengths.end(),0.0);

            stats.averageBlockLength = sum / blockLengths.size();
        }


    if (!blockCharLengths.empty())
    {
        double sum =std::accumulate(blockCharLengths.begin(),blockCharLengths.end(),0.0);

        stats.averageBlockCharLength =sum / blockCharLengths.size();
    }


    if (!blockCharLengths.empty())
    {
        std::sort(
            blockCharLengths.begin(),
            blockCharLengths.end()
        );

        int middle =
            blockCharLengths.size() / 2;

        if (blockCharLengths.size() % 2 == 0)
        {
            stats.medianBlockCharLength =
                (
                    blockCharLengths[middle - 1]
                    +
                    blockCharLengths[middle]
                ) / 2.0;
        }
        else
        {
            stats.medianBlockCharLength =
                blockCharLengths[middle];
        }
    }


    int tinyBlocksCharCounter = 0;

    int fragmentedCharCounter = 0;

    int maxTinyCharBlocks = -1;

    int mostFragmentedPathIndex = -1;

    std::vector<int> tinyPerZ(
        data.Z_all.size(),
        0
    );

    int TINY_BLOCK_CHAR_THRESHOLD = 0;

    if (FixedtinyThreshold > 0) 
    {
        TINY_BLOCK_CHAR_THRESHOLD = (int)FixedtinyThreshold;
            stats.tinyBlockCharThreshold = FixedtinyThreshold; 
    }
    else
    {


    TINY_BLOCK_CHAR_THRESHOLD =std::max(1,(int)(stats.medianBlockCharLength / 4));
        stats.tinyBlockCharThreshold =TINY_BLOCK_CHAR_THRESHOLD;
    }
    std::vector<int> tinyNodeScores(data.topo.size(),0);

    //per il relativo
    std::vector<int> totalNodeScores(data.topo.size(),0);
  
    std::cout
    << "medianBlockCharLength = "
    << stats.medianBlockCharLength
    << "\n";

std::cout
    << "tinyBlockCharThreshold = "
    << TINY_BLOCK_CHAR_THRESHOLD
    << "\n";

 for (const auto& info : blockInfos)
{

    const auto& z = data.Z_all[info.zIndex];

    // Conta tutte le occorrenze del nodo nei blocchi
    for (int pos = info.zStart; pos <= info.zEnd; pos++)
    {
        if (z[pos] == 1)
        {
            totalNodeScores[pos]++;
        }
    }

    // Conta solo le occorrenze nei tiny block
    if (info.charLength < TINY_BLOCK_CHAR_THRESHOLD)
    {
        tinyBlocksCharCounter++;

        tinyPerZ[info.zIndex]++;

        for (int pos = info.zStart; pos <= info.zEnd; pos++)
        {
            if (z[pos] == 1)
            {
                tinyNodeScores[pos]++;
            }
        }
    }
}

        std::vector<int> relativeScores(data.topo.size(),0.0);

        for (int i = 0; i < data.topo.size(); i++)
        {
            if (totalNodeScores[i] > 0)
            {
                relativeScores[i] =(int)(((double)tinyNodeScores[i] / totalNodeScores[i]) * 100.0);
            }
        }

    for (int zIndex = 0; zIndex < tinyPerZ.size(); zIndex++
    )
    {
        if ( tinyPerZ[zIndex] > FRAGMENT_CHAR_THRESHOLD)
        {
            fragmentedCharCounter++;
        }

        if (tinyPerZ[zIndex]> maxTinyCharBlocks && tinyPerZ[zIndex] > FRAGMENT_CHAR_THRESHOLD)
        {
            maxTinyCharBlocks =tinyPerZ[zIndex];

            mostFragmentedPathIndex =zIndex;
        }
    }

    stats.tinyBlocksChar =tinyBlocksCharCounter;

    stats.fragmentedPathsChar =fragmentedCharCounter;

    stats.mostFragmentedPath =mostFragmentedPathIndex;

        if (!blockCharLengths.empty())
{
    stats.tinyBlocksCharPercent =
        (
            100.0 *
            tinyBlocksCharCounter
        )
        /
        blockCharLengths.size();
}


        if (!blockLengths.empty()) {

            std::sort(blockLengths.begin(),blockLengths.end());

            int middle =blockLengths.size() / 2;

            if (blockLengths.size() % 2 == 0) {

                stats.medianBlockLength = (blockLengths[middle - 1] + blockLengths[middle]) / 2.0;
            }
            else {

                stats.medianBlockLength =blockLengths[middle];
            }
        }

        if (!blockLengths.empty()) {

            stats.maxBlockLength =*std::max_element(blockLengths.begin(),blockLengths.end());
        }

        if (!blockLengths.empty()) {

            stats.minBlockLength =*std::min_element(blockLengths.begin(),blockLengths.end());
        }



        
        // FRAGMENTED PATHS CLASSICI
        

        int fragmented = 0;

        for (const auto& mb : data.matchedBlocks) {

            if (mb.Blocks.size() > FRAGMENT_THRESHOLD) {
                fragmented++;
            }
        }

        stats.fragmentedPaths =
            fragmented;



        
        // OUTDEGREE
        

        if (!data.g.nodes.empty()) {

            double totalOutdegree = 0.0;

            for (const auto& adj : data.g.adj) {

                totalOutdegree += adj.size();
            }

            stats.averageOutdegree =
                totalOutdegree
                /
                data.g.nodes.size();
        }



        
        // PATH USAGE
        

        std::set<int> usedPaths;

        for (const auto& mb : data.matchedBlocks) {

            for (const auto& b : mb.Blocks) {

                if (b.path >= 0) 
                {
                    usedPaths.insert(b.path);
                }
            }
        }

        if (!data.MPC.empty()) 
        {
            stats.pathUsagePercent =(100.0 *usedPaths.size())/data.MPC.size();
        }
        stats.tinyNodeScores =std::move(tinyNodeScores);
        stats.relativeTinyNodeScores =std::move(relativeScores);

        return stats;
    }

    };

    #endif