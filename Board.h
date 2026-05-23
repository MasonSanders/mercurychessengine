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
	const std::array<uint64_t, static_cast<int>(Piece::PIECE_COUNT)>& getPieces() const;
	const bool& isWhiteToMove() const;
	const bool& canWhiteKingsideCastle() const;
	const bool& canWhiteQueensideCastle() const;
	const bool& canBlackKingsideCastle() const;
	const bool& canBlackQueensideCastle() const;
	const int& getEnPassantSquare() const;
	const int& getHalfmoveClock() const;
	const int& getFullmoveNum() const;

	// helpers
	uint64_t whiteOccupancy() const;
	uint64_t blackOccupancy() const;
	uint64_t allOccupancy() const;
	uint64_t sideOccupancy(bool white) const;
	uint64_t enemyOccupancy(bool white) const; 
	bool isSquareAttacked(int square, bool byWhite) const;
	bool isInCheck(bool whiteKing) const;


	// piece attacks
	uint64_t knightAttacksFrom(int square) const;
	uint64_t kingAttacksFrom(int square) const;
	uint64_t slidingAttacksFrom(
		int square, 
		uint64_t occupied, 
		const std::vector<std::pair<int, int>>& directions
	) const;
	uint64_t bishopAttacksFrom(int square) const;
	uint64_t rookAttacksFrom(int square) const;
	uint64_t queenAttacksFrom(int square) const;
	uint64_t pawnAttacksFrom(int square) const;



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
