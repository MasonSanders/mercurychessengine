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
			return static_cast<int>(Piece::WP);
		case 'N':
			return static_cast<int>(Piece::WN);
		case 'B':
			return static_cast<int>(Piece::WB);
		case 'R':
			return static_cast<int>(Piece::WR);
		case 'Q':
			return static_cast<int>(Piece::WQ);
		case 'K':
			return static_cast<int>(Piece::WK);
		case 'p':
			return static_cast<int>(Piece::BP);
		case 'n':
			return static_cast<int>(Piece::BN);
		case 'b':
			return static_cast<int>(Piece::BB);
		case 'r':
			return static_cast<int>(Piece::BR);
		case 'q':
			return static_cast<int>(Piece::BQ);
		case 'k':
			return static_cast<int>(Piece::BK);
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
	// prevent stale state.
	this->pieces.fill(0);
	this->white_to_move = true;
	this->white_kingside_castle = false;
	this->white_queenside_castle = false;
	this->black_kingside_castle = false;
	this->black_queenside_castle = false;
	this->en_passant_square = -1;
	this->halfmove_clock = 0;
	this->fullmove_num = 1;


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

			if (file > 8)
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


char Board::piece_at(int square) const {
	static constexpr std::array<char, static_cast<int>(Piece::PIECE_COUNT)> chars = {
		'P', 'N', 'B', 'R', 'Q', 'K',
		'p', 'n', 'b', 'r', 'q', 'k'
	};

	uint64_t mask = 1ULL << square;
	
	for (int p = 0; p < static_cast<int>(Piece::PIECE_COUNT); ++p) {
		if (this->pieces[p] & mask)
			return chars[p];
	}

	return '.';

}

void Board::print_board() const {
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

const std::array<uint64_t, static_cast<int>(Piece::PIECE_COUNT)>& Board::getPieces() const {
	return this->pieces;
}

const bool& Board::isWhiteToMove() const {
	return this->white_to_move;
}

const bool& Board::canWhiteKingsideCastle() const {
	return this->white_kingside_castle;
}

const bool& Board::canWhiteQueensideCastle() const {
	return this->white_queenside_castle;
}

const bool& Board::canBlackKingsideCastle() const {
	return this->black_kingside_castle;
}

const bool& Board::canBlackQueensideCastle() const {
	return this->black_queenside_castle();
}

const int& Board::getEnPassantSquare() const {
	return this->en_passant_square;
}

const int& Board::getHalfmoveClock() const {
	return this->halfmove_clock;
}

const int& Board::getFullmoveNum() const {
	return this->fullmove_num;
}
