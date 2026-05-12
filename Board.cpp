#include <stdexcept>
#include <sstream>
#include "Board.h"

Board::Board() {

}


int Board::squareIndex(int file, int rank) {
	return rank * 8 + file;
}

int Board::char_to_piece(char c) {
	switch (c) {
		case 'P':
			return Pieces::WP;
		case 'N':
			return Pieces::WN;
		case 'B':
			return Pieces::WB;
		case 'R':
			return Pieces::WR;
		case 'Q':
			return Pieces::WQ;
		case 'K':
			return Pieces::WK;
		case 'p':
			return Pieces::BP;
		case 'n':
			return Pieces::BN;
		case 'b':
			return Pieces::BB;
		case 'r':
			return Pieces::BR;
		case 'q':
			return Pieces::BQ;
		case 'k':
			return Pieces::BK;
		default:
			return -1;	
	}
}


int Board::algebraic_to_square(const std::string& square) {
	if (square == "-")
		return -1;

	if (square.size() != 2)
		throw std::runtime_error("Invalid en-passant square.");

	char file_char = square[0];
	char rank_char = square[1];
	
	if (file_char < 'a' || file_char > 'h')
		throw std::runtime_error("Invalid en-passant file.");

	if (rank_char < '1' || rank_char > '8')
		throw std::runtime_error("Invalid en-passant rank.");

	int file = file_char - 'a';
	int rank = rank_char - '1';

	return this->squareIndex(file, rank);
}

void Board::parse_fen(const std::string& fen) {
	std::stringstream iss(fen);
	std::string placement;
	std::string side_to_move;
	std::string castling;
	std::string en_passant;
	int halfmove;
	int fullmove;

	if (!(iss >> placement >> side_to_move >> castling >> en_passant >> halfmove >> fullmove))
		throw std::runtime_error("Invalid FEN, expected 6 fields.");

	int rank = 7;
	int file = 0;

	for (char c : placement) {
		if (c == '/') {
			if (file != 8)
				throw std::runtime_error("Invalid FEN, rank does not contain 8 files");

			rank--;
			file = 0;

			if (rank < 0)
				throw std::runtime_error("Invalid FEN, too many ranks");

			continue;
		}

		if (c >= '1' && c <= '8') {
			file += c - '0';

			if (file > 0)
				throw std::runtime_error("Invalid FEN, too many squares in rank");

			continue;
		}

		int piece = this->char_to_piece(c);

		if (piece == -1)
			throw std::runtime_error("unknown piece number");

		if (file >= 8)
			throw std::runtime_error("Invalid FEN, too many squares in rank");

		int sq = this->squareIndex(file, rank);

		this->pieces[piece] |= (1ULL << sq);
		file++;
	}

	if (rank != 0 || file != 0)
		throw std::runtime_error("Invalid FEN: board placement incomplete");

	if (side_to_move == "w") {
		this->white_to_move = true;
	} else if (side_to_move == "b") {
		this->white_to_move = false;
	} else {
		throw std::runtime_error("Invalid FEN, side to move must be w or b");
	}

	if (castling != "-") {
		for (char c : castling) {
			switch (c) {
				case 'K':
					this->white_kingside_castle = true;
					break;
				case 'Q':
					this->white_queenside_castle = true;
					break;
				case 'k':
					this->black_kingside_castle = true;
					break;
				case 'q':
					this->black_queenside_castle = true;
					break;
				default:
					throw std::runtime_error("Invalid FEN: bad castling rights");
			}
		}
	}

	this->en_passant_square = this->algebraic_to_square(en_passant);
	this->halfmove_clock = halfmove;
	this->fullmove_num = fullmove;
}


char Board::piece_at(int square) {
	static constexpr std::array<char, Pieces::PIECE_COUNT> chars = {
		'P', 'N', 'B', 'R', 'Q', 'K',
		'p', 'n', 'b', 'r', 'q', 'k'
	};

	uint64_t mask = 1ULL << square;
	
	for (int p = 0; p < Pieces::PIECE_COUNT; ++p) {
		if (this->pieces[p] & mask)
			return chars[p];
	}

	return '.';

}

void Board::print_board() {
	for (int rank = 7; rank >= 0; rank--) {
		std::cout << rank + 1 << "	";
		for (int file = 0; file < 8; ++file) {
			int sq = this->squareIndex(file, rank);
			std::cout << this->piece_at(sq); << ' ';
		}
		
		std::cout << "\n";
	} 
		
	std::cout << "\n	a b c d e f g h\n";
}

//getters

std::array<uint64_t, Pieces::PIECE_COUNT> Board::getPieces(){
	return this->pieces;
}

bool Board::isWhiteToMove() {
	return this->white_to_move;
}

bool Board::canWhiteKingsideCastle() {
	return this->white_kingside_castle;
}

bool Board::canWhiteQueensideCastle() {
	return this->white_queenside_castle;
}

bool Board::canBlackKingsideCastle() {
	return this->black_kingside_castle;
}

bool Board::canBlackQueensideCastle() {
	return this->black_queenside_castle();
}

int Board::getEnPassantSquare() {
	return this->en_passant_square;
}

int Board::getHalfmoveClock() {
	return this->halfmove_clock;
}

int Board::getFullmoveNum() {
	return this->fullmove_num;
}
