#ifndef BLOCKOPTIMIZER_H
#define BLOCKOPTIMIZER_H

#include "blockDecompose.hpp"
#include "graph.hpp"

#include <list>
#include <vector>

class BlockOptimizer
{
private:

    static constexpr int TINY_THRESHOLD = 35;

    static int nodeLength(
        int topoPos,
        const std::vector<int>& topo,
        const Graph& g
    )
    {
        int realNode = topo[topoPos];

        return g.nodes[realNode]
            .name.size();
    }

    static int blockCharLength(
        const longestMatchResult& b,
        const std::vector<int>& z,
        const std::vector<int>& topo,
        const Graph& g
    )
    {
        int chars = 0;

        for (
            int i = b.zStart;
            i <= b.zEnd;
            i++
        )
        {
            if (z[i] == 1)
            {
                chars +=
                    nodeLength(
                        i,
                        topo,
                        g
                    );
            }
        }

        return chars;
    }

public:

    static matchedBlock optimizeTinyBlocks(
        matchedBlock mb,
        const std::vector<int>& z,
        const std::vector<int>& topo,
        const std::vector<std::vector<int>>& delta,
        const Graph& g
    )
    {
        if (mb.Blocks.size() < 2)
            return mb;

        auto currIt =
            std::next(
                mb.Blocks.begin()
            );

        while (
            currIt != mb.Blocks.end()
        )
        {
            auto prevIt =
                std::prev(currIt);

            auto& prev = *prevIt;
            auto& curr = *currIt;
            std::cout
    << "\n=== BLOCK ===\n"
    << "prev.path=" << prev.path
    << " prev.zStart=" << prev.zStart
    << " prev.zEnd=" << prev.zEnd
    << "\n"
    << "curr.path=" << curr.path
    << " curr.zStart=" << curr.zStart
    << " curr.zEnd=" << curr.zEnd
    << "\n";

            int currChars =
                blockCharLength(
                    curr,
                    z,
                    topo,
                    g
                );

            if (
                currChars >=
                TINY_THRESHOLD
            )
            {
                ++currIt;
                continue;
            }

            int prevChars =
                blockCharLength(
                    prev,
                    z,
                    topo,
                    g
                );

            int charsToGain = 0;

            int onesToGain = 0;

            int newBoundary =
                curr.zStart;

            bool success = false;

            int pos =
                curr.zStart - 1;

          while (
    pos >= prev.zStart
)
{
    if (
        delta[curr.path][pos]
        !=
        z[pos]
    )
    {
        break;
    }

                if (z[pos] == 1)
                {
                    charsToGain +=
                        nodeLength(
                            pos,
                            topo,
                            g
                        );

                    onesToGain++;
                }

                int futureCurr =
                    currChars
                    +
                    charsToGain;

                int futurePrev =
                    prevChars
                    -
                    charsToGain;

                if (
                    futureCurr >=
                    TINY_THRESHOLD
                )
                {
                    if (
                        futurePrev >=
                        TINY_THRESHOLD
                    )
                    {
                        success = true;

                        newBoundary =
                            pos;
                    }

                    break;
                }

                pos--;
            }

            if (!success)
            {
                ++currIt;
                continue;
            }

            int oldCurrStart =
                curr.zStart;

            curr.zStart =
                newBoundary;

            prev.zEnd =
                newBoundary - 1;

            curr.length =
                curr.zEnd
                -
                curr.zStart
                +
                1;

            prev.length =
                prev.zEnd
                -
                prev.zStart
                +
                1;

            curr.matchedOnes +=
                onesToGain;

            prev.matchedOnes -=
                onesToGain;

            curr.startIndex -=
                onesToGain;

            ++currIt;
        }

        return mb;
    }
};

#endif