#ifndef __BOARD_H__
#define __BOARD_H__

#include <cstdint>
#include <array>
#include <string>
#include <vector>
#include <utility>
#include "Piece.h"
#include "Move.h"

class Board {
public:
	Board();
	int squareIndex(int file, int rank) const;
	int char_to_piece(char c);
	int algebraic_to_square(const std::string& square);
	void parse_fen(const std::string& fen);
	char piece_at(int square) const;
	int piece_index_at(int square) const;
	void remove_piece_at(int square);
	void print_board() const;
	
	
	// getters	
	const std::array<uint64_t, static_cast<int>(Piece::PIECE_COUNT)>& getPieces() const;
	const bool isWhiteToMove() const;
	const bool canWhiteKingsideCastle() const;
	const bool canWhiteQueensideCastle() const;
	const bool canBlackKingsideCastle() const;
	const bool canBlackQueensideCastle() const;
	const int getEnPassantSquare() const;
	const int getHalfmoveClock() const;
	const int getFullmoveNum() const;

	// helpers
	uint64_t whiteOccupancy() const;
	uint64_t blackOccupancy() const;
	uint64_t allOccupancy() const;
	uint64_t sideOccupancy(bool white) const;
	uint64_t enemyOccupancy(bool white) const; 
	bool isSquareAttacked(int square, bool byWhite) const;
	bool isInCheck(bool whiteKing) const;
	void addPromotionMoves(std::vector<Move>& moves, int from, int to, bool white, bool isCapture) const;


	// piece attacks
	uint64_t knightAttacksFrom(int square) const;
	uint64_t kingAttacksFrom(int square) const;
	uint64_t slidingAttacksFrom(
		int square, 
		uint64_t occupied, 
		const std::vector<std::pair<int, int>>& directions
	) const;
	uint64_t bishopAttacksFrom(int square, uint64_t occupied) const;
	uint64_t rookAttacksFrom(int square, uint64_t occupied) const;
	uint64_t queenAttacksFrom(int square, uint64_t occupied) const;
	uint64_t pawnAttacksFrom(int square, bool white) const;


	// move generation
	std::vector<Move> generatePseudoLegalMoves(bool white) const;
	void generatePawnMoves(std::vector<Move>& moves, bool white) const;
	void generateKnightMoves(std::vector<Move>& moves, bool white) const;
	void generateBishopMoves(std::vector<Move>& moves, bool white) const;
	void generateRookMoves(std::vector<Move>& moves, bool white) const;
	void generateQueenMoves(std::vector<Move>& moves, bool white) const;
	void generateKingMoves(std::vector<Move>& moves, bool white) const;
	void generateCastlingMoves(std::vector<Move>& moves, bool white) const;
	void makeMove(const Move& move);
	void updateCastlingRights(const Move& move, int movingPiece);
	std::vector<Move> generateLegalMoves() const;

	// checkmate and stalemate
	bool isCheckmate() const;
	bool isStalemate() const;




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
