#include <iostream>
#include <cstdint>
#include <sstream>
#include <string>
#include <array>
#include <stdexcept>
#include "../include/Board.h"
#include "../include/Move.h"
#include "../include/MoveType.h"
#include "../include/Engine.h"

std::string squareToString(int square) {
	char file = static_cast<char>('a' + (square % 8));
	char rank = static_cast<char>('1' + (square / 8));

	std::string result;
	result += file;
	result += rank;

	return result;
}

char promotionPieceToChar(int promotionPiece) {
	switch (static_cast<Piece>(promotionPiece)) {
        case Piece::WQ:
        case Piece::BQ:
            return 'q';
        case Piece::WR:
        case Piece::BR:
            return 'r';
        case Piece::WB:
        case Piece::BB:
            return 'b';
        case Piece::WN:
        case Piece::BN:
            return 'n';
        default:
            return '\0';
    }
}

std::string moveToString(const Move& move) {
	std::string result = squareToString(move.from) + squareToString(move.to);
	if (move.type == MoveType::Promotion ||
		move.type == MoveType::PromotionCapture) {
		char promo = promotionPieceToChar(move.promotion_piece);
		if (promo != '\0') {
			result += promo;
		}
	}
	return result;
}

int main() {
    constexpr int depth = 8;
    Engine engine;
    std::string fen;

    std::cout << "Enter a FEN position, or press Ctrl+D to quit.\n";

    while (true) {
        std::cout << "\nFEN: ";

        if (!std::getline(std::cin, fen)) {
            std::cout << '\n';
            break;
        }

        if (fen.empty()) {
            std::cerr << "Invalid FEN: input cannot be empty. Try again.\n";
            continue;
        }

        try {
            Board board;
            board.parse_fen(fen);

            std::cout << "Position:\n";
            board.print_board();

            std::vector<Move> legalMoves = board.generateLegalMoves();

            if (legalMoves.empty()) {
                std::cout << "\nLegal move count: 0\n";
                std::cout << "No legal moves available.\n";
                continue;
            }

            SearchResult result = engine.findBestMove(board, depth);

            std::cout << "\nLegal move count: " << legalMoves.size() << '\n';
            std::cout << "Best move: " << moveToString(result.bestMove) << '\n';
            std::cout << "Evaluation: " << result.evaluation << '\n';
        } catch (const std::exception& e) {
            std::cerr << "Invalid FEN or position: " << e.what() << '\n';
            std::cerr << "Please try again with a valid FEN.\n";
        }
    }

	return 0;
}
