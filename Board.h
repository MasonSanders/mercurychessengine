#ifndef __BOARD_H__
#define __BOARD_H__

#include <cstdint>
#include <array>
#include <string>
#include "Pieces.h"

class Board {
public:
	Board();
	int squareIndex(int file, int rank);
	int char_to_piece(char c);
	int algebraic_to_square(const std::string& square);
	void parse_fen(const std::string& fen);
	char piece_at(int square);
	void print_board();	
	// getters	
	std::array<uint64_t, Pieces::PIECE_COUNT> getPieces();
	bool isWhiteToMove();
	bool canWhiteKingsideCastle();
	bool canWhiteQueensideCastle();
	bool canBlackKingsideCastle();
	bool canBlackQueensideCastle();
	int getEnPassantSquare();
	int getHalfmoveClock();
	int getFullmoveNum();

private:
	std::array<uint64_t, PIECE_COUNT> pieces{};
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
