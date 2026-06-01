#include <iostream>
#include <cstdint>
#include <sstream>
#include <string>
#include <array>
#include <stdexcept>
#include "Board.h"
#include "Move.h"
#include "MoveType.h"
#include "Engine.h"

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

int main(int argc, char* argv[]) {
	Board board;

    std::string startFen =
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    try {
        board.parse_fen(startFen);

        std::cout << "Starting position:\n";
        board.print_board();

        std::vector<Move> legalMoves = board.generateLegalMoves();

        std::cout << "\nLegal move count: " << legalMoves.size() << "\n\n";

        for (const Move& move : legalMoves) {
            std::cout << moveToString(move) << '\n';
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
	return 0;
}
