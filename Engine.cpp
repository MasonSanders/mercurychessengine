#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>
#include "Engine.h"

Engine::Engine() 
    : rng(std::random_device{}()){

}

Move Engine::chooseRandomMove(const Board& board) {
    std::vector<Move> legalMoves = board.generateLegalMoves();

    if (legalMoves.empty())
        throw std::runtime_error("Cannot choose a move: no legal moves available.");

    std::uniform_int_distribution<std::size_t> distribution(
        0,
        legalMoves.size() - 1
    );

    return legalMoves[distribution(rng)];
}

int Engine::evaluateMaterial(const Board& board) const {
    static_assert(
        static_cast<int>(Piece::PIECE_COUNT) == 12,
        "Expected 12 piece bitboards"
    );

    const auto& pieces = board.getPieces();

    constexpr int FIRST_BLACK_PIECE = static_cast<int>(Piece::BP);

    int score = 0;

    for (int piece = 0; piece < static_cast<int>(Piece::PIECE_COUNT); ++piece) {
        int count = __builtin_popcountll(pieces[piece]);
        int material = PIECE_VALUES[piece] * count;

        score += piece < FIRST_BLACK_PIECE ? material : -material;
    }

    return score;
}

int Engine::evaluate(const Board& board, int plyFromRoot) const {
    if (board.isCheckmate()) {
        if (board.isWhiteToMove()) {
            return -MATE_SCORE + plyFromRoot;
        }

        return MATE_SCORE - plyFromRoot;
    }

    if (board.isStalemate()) {
        return 0;
    }

    return evaluateMaterial(board);
}

SearchResult Engine::findBestMove(const Board& board, int depth) const {
    std::vector<Move> legalMoves = board.generateLegalMoves();

    if (legalMoves.empty())
        throw std::runtime_error("Cannot find a move: no legal moves available.");

    Move bestMove = legalMoves.front();
    int bestEvaluation = board.isWhiteToMove()
        ? std::numeric_limits<int>::min()
        : std::numeric_limits<int>::max();
    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();

    for (const Move& move : legalMoves) {
        Board copy = board;
        copy.makeMove(move);

        int evaluation = minimax(copy, depth - 1, 1, alpha, beta);

        if ((board.isWhiteToMove() && evaluation > bestEvaluation) ||
            (!board.isWhiteToMove() && evaluation < bestEvaluation)) {
            bestMove = move;
            bestEvaluation = evaluation;
        }

        if (board.isWhiteToMove()) {
            alpha = std::max(alpha, bestEvaluation);
        } else {
            beta = std::min(beta, bestEvaluation);
        }
    }

    return {bestMove, bestEvaluation};
}

int Engine::minimax(const Board& board, int depth, int plyFromRoot, int alpha, int beta) const {
    if (depth <= 0)
        return evaluate(board, plyFromRoot);

    std::vector<Move> legalMoves = board.generateLegalMoves();

    if (legalMoves.empty())
        return evaluate(board, plyFromRoot);

    int bestEvaluation = board.isWhiteToMove()
        ? std::numeric_limits<int>::min()
        : std::numeric_limits<int>::max();

    for (const Move& move : legalMoves) {
        Board copy = board;
        copy.makeMove(move);

        int evaluation = minimax(copy, depth - 1, plyFromRoot + 1, alpha, beta);

        if (board.isWhiteToMove()) {
            bestEvaluation = std::max(bestEvaluation, evaluation);
            alpha = std::max(alpha, bestEvaluation);
        } else {
            bestEvaluation = std::min(bestEvaluation, evaluation);
            beta = std::min(beta, bestEvaluation);
        }

        if (beta <= alpha)
            break;
    }

    return bestEvaluation;
}
