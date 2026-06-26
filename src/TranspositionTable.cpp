#include <algorithm>
#include "../include/TranspositionTable.h"
#include "../include/Piece.h"

void TranspositionTable::clear() {
    entries.clear();
}

std::optional<TTEntry> TranspositionTable::probe(
    const Board& board,
    int depth,
    int& alpha,
    int& beta
) const {
    auto found = entries.find(hashBoard(board));

    if (found == entries.end() || found->second.depth < depth) {
        return std::nullopt;
    }

    const TTEntry& entry = found->second;

    if (entry.flag == TTFlag::Exact) {
        return entry;
    }

    if (entry.flag == TTFlag::LowerBound) {
        alpha = std::max(alpha, entry.evaluation);
    } else if (entry.flag == TTFlag::UpperBound) {
        beta = std::min(beta, entry.evaluation);
    }

    if (alpha >= beta) {
        return entry;
    }

    return std::nullopt;
}

void TranspositionTable::store(
    const Board& board,
    int depth,
    int evaluation,
    int originalAlpha,
    int originalBeta
) {
    TTFlag flag = TTFlag::Exact;

    if (evaluation <= originalAlpha) {
        flag = TTFlag::UpperBound;
    } else if (evaluation >= originalBeta) {
        flag = TTFlag::LowerBound;
    }

    entries[hashBoard(board)] = {depth, evaluation, flag};
}

uint64_t TranspositionTable::hashBoard(const Board& board) const {
    uint64_t hash = 1469598103934665603ULL;

    auto mix = [&](uint64_t value) {
        hash ^= value;
        hash *= 1099511628211ULL;
    };

    const auto& pieces = board.getPieces();

    for (int i = 0; i < static_cast<int>(Piece::PIECE_COUNT); ++i) {
        mix(pieces[i] + 0x9e3779b97f4a7c15ULL + static_cast<uint64_t>(i));
    }

    mix(board.isWhiteToMove());
    mix(board.canWhiteKingsideCastle());
    mix(board.canWhiteQueensideCastle());
    mix(board.canBlackKingsideCastle());
    mix(board.canBlackQueensideCastle());
    mix(static_cast<uint64_t>(board.getEnPassantSquare() + 1));

    return hash;
}
