#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <random>
#include <array>
#include "Board.h"
#include "Move.h"

class Engine {
public:
    Engine();

    Move chooseRandomMove(const Board& board);

    int evaluateMaterial(const Board& board) const;
    int evaluate(const Board& board, int plyFromRoot = 0) const;

private:
    std::mt19937 rng;

    static constexpr int MATE_SCORE = 100000;

    static constexpr std::array<int, static_cast<int>(Piece::PIECE_COUNT)> PIECE_VALUES = {
        100, 320, 330, 500, 900, 0,
        100, 320, 330, 500, 900, 0
    };
};


#endif