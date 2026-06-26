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
    (void)plyFromRoot;
    return evaluateMaterial(board);
}

SearchResult Engine::findBestMove(const Board& board, int depth) const {
    transpositionTable.clear();

    std::vector<Move> legalMoves = board.generateLegalMoves();

    if (legalMoves.empty())
        throw std::runtime_error("Cannot find a move: no legal moves available.");

    orderMoves(legalMoves, board);

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

    int originalAlpha = alpha;
    int originalBeta = beta;

    if (std::optional<TTEntry> entry = transpositionTable.probe(board, depth, alpha, beta)) {
        return entry->evaluation;
    }

    std::vector<Move> legalMoves = board.generateLegalMoves();

    if (legalMoves.empty()) {
        if (board.isInCheck(board.isWhiteToMove())) {
            return board.isWhiteToMove()
                ? -MATE_SCORE + plyFromRoot
                : MATE_SCORE - plyFromRoot;
        }

        return 0;
    }

    orderMoves(legalMoves, board);

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

    transpositionTable.store(board, depth, bestEvaluation, originalAlpha, originalBeta);

    return bestEvaluation;
}

void Engine::orderMoves(std::vector<Move>& moves, const Board& board) const {
    std::sort(moves.begin(), moves.end(), [&](const Move& a, const Move& b) {
        return moveOrderingScore(board, a) > moveOrderingScore(board, b);
    });
}

int Engine::moveOrderingScore(const Board& board, const Move& move) const {
    int score = 0;

    if (move.type == MoveType::Promotion ||
        move.type == MoveType::PromotionCapture) {
        score += PIECE_VALUES[move.promotion_piece] + 8000;
    }

    if (move.type == MoveType::Capture ||
        move.type == MoveType::PromotionCapture ||
        move.type == MoveType::EnPassant) {
        int attacker = board.piece_index_at(move.from);
        int victim = board.piece_index_at(move.to);

        if (move.type == MoveType::EnPassant) {
            victim = board.isWhiteToMove()
                ? static_cast<int>(Piece::BP)
                : static_cast<int>(Piece::WP);
        }

        if (attacker != -1 && victim != -1) {
            score += 10000 + (10 * PIECE_VALUES[victim]) - PIECE_VALUES[attacker];
        }
    }

    if (move.type == MoveType::KingCastle ||
        move.type == MoveType::QueenCastle) {
        score += 50;
    }

    return score;
}
