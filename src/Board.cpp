#include <stdexcept>
#include <sstream>
#include <iostream>
#include "../include/Board.h"
#include "../include/BitboardUtils.h"

Board::Board() {

}


int Board::squareIndex(int file, int rank) const {
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

	if (rank != 0 || file != 8)
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

int Board::piece_index_at(int square) const {
	uint64_t mask = bit(square);

	for (int p = 0; p < static_cast<int>(Piece::PIECE_COUNT); ++p) {
		if (pieces[p] & mask)
			return p;
	}

	return -1;
}

void Board::remove_piece_at(int square) {
	uint64_t mask = bit(square);

	for (int p = 0; p < static_cast<int>(Piece::PIECE_COUNT); ++p) {
		pieces[p] &= ~mask;
	}
}

void Board::print_board() const {
	for (int rank = 7; rank >= 0; rank--) {
		std::cout << rank + 1 << "	";
		for (int file = 0; file < 8; ++file) {
			int sq = this->squareIndex(file, rank);
			std::cout << this->piece_at(sq) << ' ';
		}
		
		std::cout << "\n";
	} 
		
	std::cout << "\n	a b c d e f g h\n";
}

//getters

const std::array<uint64_t, static_cast<int>(Piece::PIECE_COUNT)>& Board::getPieces() const {
	return this->pieces;
}

bool Board::isWhiteToMove() const {
	return this->white_to_move;
}

bool Board::canWhiteKingsideCastle() const {
	return this->white_kingside_castle;
}

bool Board::canWhiteQueensideCastle() const {
	return this->white_queenside_castle;
}

bool Board::canBlackKingsideCastle() const {
	return this->black_kingside_castle;
}

bool Board::canBlackQueensideCastle() const {
	return this->black_queenside_castle;
}

int Board::getEnPassantSquare() const {
	return this->en_passant_square;
}

int Board::getHalfmoveClock() const {
	return this->halfmove_clock;
}

int Board::getFullmoveNum() const {
	return this->fullmove_num;
}


uint64_t Board::whiteOccupancy() const {
	return pieces[static_cast<int>(Piece::WP)] |
		   pieces[static_cast<int>(Piece::WN)] |
		   pieces[static_cast<int>(Piece::WB)] |
		   pieces[static_cast<int>(Piece::WR)] |
		   pieces[static_cast<int>(Piece::WQ)] |
		   pieces[static_cast<int>(Piece::WK)];
}

uint64_t Board::blackOccupancy() const {
	return pieces[static_cast<int>(Piece::BP)] |
		   pieces[static_cast<int>(Piece::BN)] |
		   pieces[static_cast<int>(Piece::BB)] |
		   pieces[static_cast<int>(Piece::BR)] |
		   pieces[static_cast<int>(Piece::BQ)] |
		   pieces[static_cast<int>(Piece::BK)];
}

uint64_t Board::allOccupancy() const {
	return this->whiteOccupancy() | this->blackOccupancy();
}

uint64_t Board::sideOccupancy(bool white) const {
	return white ? this->whiteOccupancy() : this->blackOccupancy();
}

uint64_t Board::enemyOccupancy(bool white) const {
	return white ? this->blackOccupancy() : this->whiteOccupancy();
}

bool Board::isSquareAttacked(int square, bool byWhite) const {
	uint64_t occupied = allOccupancy();
	uint64_t target = bit(square);

	int knightPiece = byWhite ? static_cast<int>(Piece::WN) : static_cast<int>(Piece::BN);
	int bishopPiece = byWhite ? static_cast<int>(Piece::WB) : static_cast<int>(Piece::BB);
	int rookPiece = byWhite ? static_cast<int>(Piece::WR) : static_cast<int>(Piece::BR);
	int queenPiece = byWhite ? static_cast<int>(Piece::WQ) : static_cast<int>(Piece::BQ);
	int kingPiece = byWhite ? static_cast<int>(Piece::WK) : static_cast<int>(Piece::BK);
	int pawnPiece = byWhite ? static_cast<int>(Piece::WP) : static_cast<int>(Piece::BP);

	auto anyAttacksTarget = [&](uint64_t attackers, auto attacksFrom) -> bool {
		while (attackers) {
			int from = __builtin_ctzll(attackers);
			attackers &= attackers - 1;

			if (attacksFrom(from) & target) {
				return true;
			}
		}
		return false;
	};

	if (anyAttacksTarget(pieces[knightPiece], [&](int from) {
		return knightAttacksFrom(from);
	})) {
		return true;
	}

	if (anyAttacksTarget(pieces[bishopPiece], [&](int from) {
		return bishopAttacksFrom(from, occupied);
	})) {
		return true;
	}

	if (anyAttacksTarget(pieces[rookPiece], [&](int from) {
		return rookAttacksFrom(from, occupied);
	})) {
		return true;
	}
	
	if (anyAttacksTarget(pieces[queenPiece], [&](int from) {
		return queenAttacksFrom(from, occupied);
	})) {
		return true;
	}

	if (anyAttacksTarget(pieces[kingPiece], [&](int from) {
		return kingAttacksFrom(from);
	})) {
		return true;
	}

	if (anyAttacksTarget(pieces[pawnPiece], [&](int from) {
		return pawnAttacksFrom(from, byWhite);
	})) {
		return true;
	}
	return false;
}

bool Board::isInCheck(bool whiteKing) const {
	int kingPiece = whiteKing ? static_cast<int>(Piece::WK) : static_cast<int>(Piece::BK);
	uint64_t kingBoard = this->pieces[kingPiece];

	if (kingBoard == 0) {
		throw std::runtime_error("King missing from board");
	}

	// returns the number of trailing 0 bits in an unsigned long long
	int kingSquare = __builtin_ctzll(kingBoard);
	return isSquareAttacked(kingSquare, !whiteKing);
}




uint64_t Board::knightAttacksFrom(int square) const {
	uint64_t attacks = 0;

	int f = file_of(square);
	int r = rank_of(square);

	const int df[8] = {1, 2, 2, 1, -1, -2, -2, -1};
	const int dr[8] = {2, 1, -1, -2, -2, -1, 1, 2};

	for (int i = 0; i < 8; i++) {
		int nf = f + df[i];
		int nr = r + dr[i];

		if (nf >= 0 && nf < 8 && nr >= 0 && nr < 8)
			attacks |= bit(nr * 8 + nf);
	}

	return attacks;
}

uint64_t Board::kingAttacksFrom(int square) const {
	uint64_t attacks = 0;
	int f = file_of(square);
	int r = rank_of(square);

	const int df[8] = {1, 1, 0, -1, -1, -1, 0, 1};
	const int dr[8] = {0, 1, 1, 1, 0, -1, -1, -1};

	for (int i = 0; i < 8; i++) {
		int nf = f + df[i];
		int nr = r + dr[i];
		
		if (nf >= 0 && nf < 8 && nr >= 0 && nr < 8)
			attacks |= bit(nr * 8 + nf);
	}
	return attacks;
}

uint64_t Board::slidingAttacksFrom(
	int square,
	uint64_t occupied,
	const std::vector<std::pair<int, int>>& directions
) const {
	uint64_t attacks = 0;
	int startFile = file_of(square);
	int startRank = rank_of(square);

	for (auto [df, dr] : directions) {
		int file = startFile + df;
		int rank = startRank + dr;

		while (file >= 0 && file < 8 && rank >= 0 && rank < 8) {
			int target = rank * 8 + file;

			attacks |= bit(target);

			if(is_set(occupied, target))
				break;

			file += df;
			rank += dr;

		}
	}
	return attacks;
}

uint64_t Board::bishopAttacksFrom(int square, uint64_t occupied) const {
	static const std::vector<std::pair<int, int>> dirs = {
		{1, 1},
		{-1, 1},
		{1, -1},
		{-1, -1}
	};

	return slidingAttacksFrom(square, occupied, dirs);
}

uint64_t Board::rookAttacksFrom(int square, uint64_t occupied) const {
	static const std::vector<std::pair<int, int>> dirs = {
		{1, 0},
		{-1, 0},
		{0, 1},
		{0, -1}
	};

	return slidingAttacksFrom(square, occupied, dirs);
}

uint64_t Board::queenAttacksFrom(int square, uint64_t occupied) const {
	return bishopAttacksFrom(square, occupied) |
		   rookAttacksFrom(square, occupied);
}

uint64_t Board::pawnAttacksFrom(int square, bool white) const {
	uint64_t attacks = 0;

	int file = file_of(square);

	auto addIfValid = [&](int target) {
		if (target >= 0 && target < 64)
			attacks |= bit(target);
	}; 

	if (white) {
		if (file > 0)
			addIfValid(square + 7);

		if (file < 7)
			addIfValid(square + 9);
	} else {
		if (file > 0)
			addIfValid(square - 9);

		if (file < 7)
			addIfValid(square - 7);
	}
	return attacks;
}

void Board::addPromotionMoves(std::vector<Move>& moves, int from, int to, bool white, bool isCapture) const {
	MoveType type = isCapture ? MoveType::PromotionCapture : MoveType::Promotion;

	if (white) {
		moves.push_back({from, to, type, static_cast<int>(Piece::WQ)});
		moves.push_back({from, to, type, static_cast<int>(Piece::WR)});
		moves.push_back({from, to, type, static_cast<int>(Piece::WB)});
		moves.push_back({from, to, type, static_cast<int>(Piece::WN)});
	} else {
		moves.push_back({from, to, type, static_cast<int>(Piece::BQ)});
		moves.push_back({from, to, type, static_cast<int>(Piece::BR)});
		moves.push_back({from, to, type, static_cast<int>(Piece::BB)});
		moves.push_back({from, to, type, static_cast<int>(Piece::BN)});
	}
}


// move generation

std::vector<Move> Board::generatePseudoLegalMoves(bool white) const {
	std::vector<Move> moves;
    moves.reserve(64);
        
	generatePawnMoves(moves, white);
	generateKnightMoves(moves, white);
	generateBishopMoves(moves, white);
	generateRookMoves(moves, white);
	generateQueenMoves(moves, white);
	generateKingMoves(moves, white);

	return moves;
}

void Board::generatePawnMoves(std::vector<Move>& moves, bool white) const {
	int pawnPiece = white ? static_cast<int>(Piece::WP) : static_cast<int>(Piece::BP);

	uint64_t pawns = this->pieces[pawnPiece];
	uint64_t all = this->allOccupancy();
	uint64_t enemy = this->enemyOccupancy(white);

	int direction = white ? 8 : -8;
	int startRank = white ? 1 : 6;
	int promotionFromRank = white ? 6 : 1;

	while (pawns) {
		int from = __builtin_ctzll(pawns);
		pawns &= pawns - 1;

		int rank = rank_of(from);
		int oneForward = from + direction;

		if (oneForward >= 0 && oneForward < 64 && !is_set(all, oneForward)) {
			if (rank == promotionFromRank) {
				addPromotionMoves(moves, from, oneForward, white, false);
			} else {
				moves.push_back({from, oneForward, MoveType::Quiet, -1});
				int twoForward = from + 2 * direction;
				if (rank == startRank &&
					twoForward >= 0 && twoForward < 64 &&
					!is_set(all, twoForward)) {
					moves.push_back({from, twoForward, MoveType::DoublePawnPush, -1});
				}
			}
		}

		uint64_t attacks = pawnAttacksFrom(from, white);
		uint64_t captures = attacks & enemy;

		while (captures) {
			int to = __builtin_ctzll(captures);
			captures &= captures -1;

			if (rank == promotionFromRank) {
				addPromotionMoves(moves, from, to, white, true);
			} else {
				moves.push_back({from, to, MoveType::Capture, -1});
			}
		}

		if (en_passant_square != -1) {
			uint64_t epTarget = bit(en_passant_square);

			if (attacks & epTarget) {
				moves.push_back({from, en_passant_square, MoveType::EnPassant, -1});
			}
		}

	}
}

void Board::generateKnightMoves(std::vector<Move>& moves, bool white) const {
	int knightPiece = white ? static_cast<int>(Piece::WN) : static_cast<int>(Piece::BN);

	uint64_t knights = pieces[knightPiece];
	uint64_t own = sideOccupancy(white);
	uint64_t enemy = enemyOccupancy(white);

	while (knights) {
		int from = __builtin_ctzll(knights);
		knights &= knights - 1;

		uint64_t targets = knightAttacksFrom(from) & ~own;

		while (targets) {
			int to = __builtin_ctzll(targets);
			targets &= targets - 1;
			
			MoveType type = is_set(enemy, to) ? MoveType::Capture : MoveType::Quiet;

			moves.push_back({from, to, type, -1});
		}
	}
}

void Board::generateBishopMoves(std::vector<Move>& moves, bool white) const {
	int bishopPiece = white ? static_cast<int>(Piece::WB) : static_cast<int>(Piece::BB);

	uint64_t bishops = pieces[bishopPiece];
	uint64_t own = sideOccupancy(white);
	uint64_t enemy = enemyOccupancy(white);
	uint64_t occupied = allOccupancy();

	while (bishops) {
		int from = __builtin_ctzll(bishops);
		bishops &= bishops - 1;

		uint64_t targets = bishopAttacksFrom(from, occupied) & ~own;

		while (targets) {
			int to = __builtin_ctzll(targets);
			targets &= targets - 1;
			
			MoveType type = is_set(enemy, to) ? MoveType::Capture : MoveType::Quiet;

			moves.push_back({from, to, type, -1});
		}
	}
}

void Board::generateRookMoves(std::vector<Move>& moves, bool white) const {
	int rookPiece = white ? static_cast<int>(Piece::WR) : static_cast<int>(Piece::BR);

	uint64_t rooks = pieces[rookPiece];
	uint64_t own = sideOccupancy(white);
	uint64_t enemy = enemyOccupancy(white);
	uint64_t occupied = allOccupancy();

	while (rooks) {
		int from = __builtin_ctzll(rooks);
		rooks &= rooks - 1;

		uint64_t targets = rookAttacksFrom(from, occupied) & ~own;

		while (targets) {
			int to = __builtin_ctzll(targets);
			targets &= targets - 1;
			
			MoveType type = is_set(enemy, to) ? MoveType::Capture : MoveType::Quiet;

			moves.push_back({from, to, type, -1});
		}
	}
}

void Board::generateQueenMoves(std::vector<Move>& moves, bool white) const {
	int queenPiece = white ? static_cast<int>(Piece::WQ) : static_cast<int>(Piece::BQ);

	uint64_t queens = pieces[queenPiece];
	uint64_t own = sideOccupancy(white);
	uint64_t enemy = enemyOccupancy(white);
	uint64_t occupied = allOccupancy();

	while (queens) {
		int from = __builtin_ctzll(queens);
		queens &= queens - 1;

		uint64_t targets = queenAttacksFrom(from, occupied) & ~own;

		while (targets) {
			int to = __builtin_ctzll(targets);
			targets &= targets - 1;
			
			MoveType type = is_set(enemy, to) ? MoveType::Capture : MoveType::Quiet;

			moves.push_back({from, to, type, -1});
		}
	}
}

void Board::generateKingMoves(std::vector<Move>& moves, bool white) const {
	int kingPiece = white ? static_cast<int>(Piece::WK) : static_cast<int>(Piece::BK);

	uint64_t kings = pieces[kingPiece];
	uint64_t own = sideOccupancy(white);
	uint64_t enemy = enemyOccupancy(white);

	while (kings) {
		int from = __builtin_ctzll(kings);
		kings &= kings - 1;

		uint64_t targets = kingAttacksFrom(from) & ~own;

		while (targets) {
			int to = __builtin_ctzll(targets);
			targets &= targets - 1;
			
			MoveType type = is_set(enemy, to) ? MoveType::Capture : MoveType::Quiet;

			moves.push_back({from, to, type, -1});
		}
	}

	generateCastlingMoves(moves, white);
}

void Board::generateCastlingMoves(std::vector<Move>& moves, bool white) const {
	uint64_t occupied = allOccupancy();

	if (white) {
		constexpr int E1 = 4;
		constexpr int F1 = 5;
		constexpr int G1 = 6;
		constexpr int H1 = 7;
		constexpr int A1 = 0;
		constexpr int B1 = 1;
		constexpr int C1 = 2;
		constexpr int D1 = 3;

		bool whiteKingOnE1 = is_set(pieces[static_cast<int>(Piece::WK)], E1);
		bool whiteRookOnH1 = is_set(pieces[static_cast<int>(Piece::WR)], H1);
		bool whiteRookOnA1 = is_set(pieces[static_cast<int>(Piece::WR)], A1);

		if (white_kingside_castle &&
			whiteKingOnE1 &&
			whiteRookOnH1 &&
			!is_set(occupied, F1) &&
			!is_set(occupied, G1) &&
			!isSquareAttacked(E1, false) &&
			!isSquareAttacked(F1, false) &&
			!isSquareAttacked(G1, false)) {
			moves.push_back({E1, G1, MoveType::KingCastle, -1});
		}

		if (white_queenside_castle &&
			whiteKingOnE1 &&
			whiteRookOnA1 &&
			!is_set(occupied, B1) &&
			!is_set(occupied, C1) &&
			!is_set(occupied, D1) &&
			!isSquareAttacked(E1, false) &&
			!isSquareAttacked(D1, false) &&
			!isSquareAttacked(C1, false)) {
			moves.push_back({E1, C1, MoveType::QueenCastle, -1});
		}
	} else {
		constexpr int E8 = 60;
		constexpr int F8 = 61;
		constexpr int G8 = 62;
		constexpr int H8 = 63;
		constexpr int A8 = 56;
		constexpr int B8 = 57;
		constexpr int C8 = 58;
		constexpr int D8 = 59;

		bool blackKingOnE8 = is_set(pieces[static_cast<int>(Piece::BK)], E8);
		bool blackRookOnH8 = is_set(pieces[static_cast<int>(Piece::BR)], H8);
		bool blackRookOnA8 = is_set(pieces[static_cast<int>(Piece::BR)], A8);

		if (black_kingside_castle &&
			blackKingOnE8 &&
			blackRookOnH8 &&
			!is_set(occupied, F8) &&
			!is_set(occupied, G8) &&
			!isSquareAttacked(E8, true) &&
			!isSquareAttacked(F8, true) &&
			!isSquareAttacked(G8, true)) {
			moves.push_back({E8, G8, MoveType::KingCastle, -1});
		}

		if (black_queenside_castle &&
			blackKingOnE8 &&
			blackRookOnA8 &&
			!is_set(occupied, B8) &&
			!is_set(occupied, C8) &&
			!is_set(occupied, D8) &&
			!isSquareAttacked(E8, true) &&
			!isSquareAttacked(D8, true) &&
			!isSquareAttacked(C8, true)) {
			moves.push_back({E8, C8, MoveType::QueenCastle, -1});
		} 
	}
}

void Board::makeMove(const Move& move) {
	int movingPiece = piece_index_at(move.from);

	if (movingPiece == -1)
		throw std::runtime_error("No piece on source square");

	pieces[movingPiece] &= ~bit(move.from);
	remove_piece_at(move.to);

	if (move.type == MoveType::EnPassant) {
		int capturedPawnSquare = white_to_move ? move.to - 8 : move.to + 8;
		remove_piece_at(capturedPawnSquare); 
	}

	if (move.type == MoveType::Promotion ||
		move.type == MoveType::PromotionCapture) {
		pieces[move.promotion_piece] |= bit(move.to);
	} else {
		pieces[movingPiece] |= bit(move.to);
	}

	if (move.type == MoveType::KingCastle) {
		if (white_to_move) {
			pieces[static_cast<int>(Piece::WR)] &= ~bit(7);
			pieces[static_cast<int>(Piece::WR)] |= bit(5);
		} else {
			pieces[static_cast<int>(Piece::BR)] &= ~bit(63);
			pieces[static_cast<int>(Piece::BR)] |= bit(61);
		}
	} else if (move.type == MoveType::QueenCastle) {
		if (white_to_move) {
			pieces[static_cast<int>(Piece::WR)] &= ~bit(0);
			pieces[static_cast<int>(Piece::WR)] |= bit(3);
		} else {
			pieces[static_cast<int>(Piece::BR)] &= ~bit(56);
			pieces[static_cast<int>(Piece::BR)] |= bit(59);
		}
	}

	updateCastlingRights(move, movingPiece);

	en_passant_square = -1;

	if (move.type == MoveType::DoublePawnPush) {
		en_passant_square = white_to_move ? move.from + 8 : move.from - 8;
	}

	white_to_move = !white_to_move;
}

void Board::updateCastlingRights(const Move& move, int movingPiece) {
	if (movingPiece == static_cast<int>(Piece::WK)) {
		white_kingside_castle = false;
		white_queenside_castle = false;
	}
	if (movingPiece == static_cast<int>(Piece::BK)) {
		black_kingside_castle = false;
		black_queenside_castle = false;
	}

	if (movingPiece == static_cast<int>(Piece::WR)) {
		if (move.from == 7)
			white_kingside_castle = false;
		if (move.from == 0)
			white_queenside_castle = false;
	}

	if (movingPiece == static_cast<int>(Piece::BR)) {
		if (move.from == 63)
			black_kingside_castle = false;
		if (move.from == 56)
			black_queenside_castle = false;
	}

	if (move.to == 7)
		white_kingside_castle = false;
	if (move.to == 0)
		white_queenside_castle = false;
	if (move.to == 63)
		black_kingside_castle = false;
	if (move.to == 56)
		black_queenside_castle = false;
}

std::vector<Move> Board::generateLegalMoves() const {
	bool movingSide = white_to_move;
	std::vector<Move> pseudo = generatePseudoLegalMoves(movingSide);
	std::vector<Move> legal;
    legal.reserve(pseudo.size());
    
	for (const Move& move : pseudo) {
		Board copy = *this;
		copy.makeMove(move);

		if (!copy.isInCheck(movingSide))
			legal.push_back(move);
	}
	return legal;
}

bool Board::isCheckmate() const {
	return isInCheck(white_to_move) && generateLegalMoves().empty();
}

bool Board::isStalemate() const {
	return !isInCheck(white_to_move) && generateLegalMoves().empty();
}

uint64_t Board::perft(int depth) const {
	if (depth == 0)
		return 1;

	uint64_t nodes = 0;
	std::vector<Move> moves = generateLegalMoves();

	for (const Move& move : moves) {
		Board copy = *this;
		copy.makeMove(move);
		nodes += copy.perft(depth - 1);
	}

	return nodes;
}
