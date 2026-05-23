#ifndef __BOARD_H__
#define __BOARD_H__

#include <cstdint>
#include <array>
#include <string>
#include "Piece.h"

class Board {
public:
	Board();
	int squareIndex(int file, int rank);
	int char_to_piece(char c);
	int algebraic_to_square(const std::string& square);
	void parse_fen(const std::string& fen);
	char piece_at(int square) const;
	void print_board() const;	
	// getters	
	const std::array<uint64_t, static_cast<int>(Piece::PIECE_COUNT)>& getPieces();
	const bool& isWhiteToMove() const;
	const bool& canWhiteKingsideCastle() const;
	const bool& canWhiteQueensideCastle() const;
	const bool& canBlackKingsideCastle() const;
	const bool& canBlackQueensideCastle() const;
	const int& getEnPassantSquare() const;
	const int& getHalfmoveClock() const;
	const int& getFullmoveNum() const;

private:
	std::array<uint64_t, static_cast<int>(Piece::PIECE_COUNT)> pieces{};
	bool white_to_move = true;
	bool white_kingside_castle = false;
	bool white_queenside_castle = false;
	bool black_kingside_castle = false;
	bool black_queenside_castle = false;
	int en_passant_square = -1;
	int halfmove_clock = 0;
	int fullmove_num = 1;
};

#endif
