#ifndef __TRANSPOSITION_TABLE_H__
#define __TRANSPOSITION_TABLE_H__

#include <cstdint>
#include <optional>
#include <unordered_map>
#include "Board.h"

enum class TTFlag {
    Exact,
    LowerBound,
    UpperBound
};

struct TTEntry {
    int depth;
    int evaluation;
    TTFlag flag;
};

class TranspositionTable {
public:
    void clear();

    std::optional<TTEntry> probe(
        const Board& board,
        int depth,
        int& alpha,
        int& beta
    ) const;

    void store(
        const Board& board,
        int depth,
        int evaluation,
        int originalAlpha,
        int originalBeta
    );

private:
    uint64_t hashBoard(const Board& board) const;

    std::unordered_map<uint64_t, TTEntry> entries;
};

#endif
