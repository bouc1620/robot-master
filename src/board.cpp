#include <initializer_list>
#include <sstream>
#include <string>

#include "zobrist-hash.h"
#include "bitboard.h"
#include "types.h"
#include "board.h"
#include "util.h"

Board::Board(std::string FEN) :
	pieces{},
	allPieces{},
	toMove{},
	castle{},
	enPassant{},
	halfmoveClock{},
	fullmoveNumber{},
	zhash{},
	zkey{} {
	setup(FEN);
}

std::pair<Color, Piece> Board::findPiece(int square, Color color) const {
	if (color != NO_COLOR) {
		for (Piece piece : { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING }) {
			if (pieces[color][piece] & (ONE << square)) {
				return { color, piece };
			}
		}
	} else {
		for (Piece piece : { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING }) {
			if (pieces[WHITE][piece] & (ONE << square)) {
				return { WHITE, piece };
			} else if (pieces[BLACK][piece] & (ONE << square)) {
				return { BLACK, piece };
			}
		}
	}

	return { NO_COLOR, NO_PIECE };
}

std::string Board::toString() const {
	std::string result = "";

	for (int square = 56; square >= 0; square++) {
		auto [color, piece] = findPiece(square);

		if (piece == NO_PIECE) {
			result += ". ";
		} else {
			result += FEN::PIECE_CHARS[color][piece];
			result += " ";
		}

		if ((square + 1) % 8 == 0) {
			result += "\n";
			square += 2 * -8;
		}
	}

	return result;
}

const U64(&Board::getPieces() const)[2][6]{
	return pieces;
}

const U64(&Board::getAllPieces() const)[2]{
	return allPieces;
}

Color Board::getToMove() const {
	return toMove;
}

const bool(&Board::getCastle() const)[2][2]{
	return castle;
}

int Board::getEnPassant() const {
	return enPassant;
}

unsigned int Board::getHalfmoveClock() const {
	return halfmoveClock;
}

unsigned int Board::getFullmoveNumber() const {
	return fullmoveNumber;
}

U64 Board::getZkey() const {
	return zkey;
}

void Board::setup(std::string FEN) {
	std::istringstream ss(FEN);
	std::string position,
		toMove_,
		castle_,
		enPassant_,
		halfmoveClock_,
		fullmoveNumber_;

	ss >> position;

	int square = 56;
	for (int i = 0; i < position.length(); i++) {
		if (position[i] == '/') {
			square += 2 * -8;
		} else if (isdigit(position[i])) {
			square += position[i] - '0';
		} else {
			for (Piece piece : { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING }) {
				if (position[i] == FEN::PIECE_CHARS[WHITE][piece]) {
					putPiece(square, WHITE, piece);
					break;
				} else if (position[i] == FEN::PIECE_CHARS[BLACK][piece]) {
					putPiece(square, BLACK, piece);
					break;
				}
			}

			square++;
		}
	}

	ss >> toMove_;

	toMove = toMove_[0] == FEN::COLOR_CHARS[WHITE] ? WHITE : BLACK;

	if (toMove == BLACK) {
		zkey ^= zhash.getToMove();
	}

	ss >> castle_;

	for (Castle side : { KINGSIDE, QUEENSIDE }) {
		castle[WHITE][side] = castle_.find(FEN::CASTLE_CHARS[WHITE][side]) != std::string::npos;
		castle[BLACK][side] = castle_.find(FEN::CASTLE_CHARS[BLACK][side]) != std::string::npos;
	}

	for (Castle side : { KINGSIDE, QUEENSIDE }) {
		if (castle[WHITE][side]) {
			zkey ^= zhash.getCastle()[WHITE][side];
		}

		if (castle[BLACK][side]) {
			zkey ^= zhash.getCastle()[BLACK][side];
		}
	}

	ss >> enPassant_;

	if (enPassant_[0] == '-') {
		enPassant = NO_SQUARE;
	} else {
		enPassant = square_stoi(enPassant_);
	}

	if (enPassant != NO_SQUARE) {
		zkey ^= zhash.getEnPassant()[getFile(enPassant)];
	}

	ss >> halfmoveClock_;

	if (halfmoveClock_.size() == 0 || halfmoveClock_[0] == '-') {
		halfmoveClock = 0;
	} else {
		halfmoveClock = std::stoi(halfmoveClock_);
	}

	ss >> fullmoveNumber_;

	if (fullmoveNumber_.size() == 0 || fullmoveNumber_[0] == '-') {
		fullmoveNumber = 1;
	} else {
		fullmoveNumber = std::stoi(fullmoveNumber_);
	}
}

void Board::removeCastle(Color color, Castle side) {
	castle[color][side] = false;

	zkey ^= zhash.getCastle()[color][side];
}

void Board::removeCastles(Color color) {
	for (Castle side : { KINGSIDE, QUEENSIDE }) {
		if (castle[toMove][side]) {
			castle[toMove][side] = false;

			zkey ^= zhash.getCastle()[toMove][side];
		}
	}
}

void Board::putPiece(int square, Color color, Piece piece) {
	pieces[color][piece] |= ONE << square;

	allPieces[color] |= ONE << square;

	zkey ^= zhash.getPieces()[color][piece][square];
}

void Board::removePiece(int square, Color color, Piece piece) {
	pieces[color][piece] &= ~(ONE << square);

	allPieces[color] &= ~(ONE << square);

	zkey ^= zhash.getPieces()[color][piece][square];
}

void Board::doMove(Move move) {
	static const int ROOK_SQUARES[2][2] =
	{
		{	0,	7 },
		{ 56, 63 }
	};

	if (enPassant != NO_SQUARE) {
		zkey ^= zhash.getEnPassant()[getFile(enPassant)];

		enPassant = NO_SQUARE;
	}

	Color opponent = opposite(toMove);

	int startSquare = move.getStartSquare();

	auto [color, piece] = findPiece(startSquare, toMove);
	removePiece(startSquare, toMove, piece);

	int endSquare = move.getEndSquare();

	if (move.isNormal()) {
		auto [capturedColor, capturedPiece] = findPiece(endSquare, opponent);

		if (capturedPiece != NO_PIECE) {
			removePiece(endSquare, opponent, capturedPiece);

			if (capturedPiece == ROOK) {
				for (Castle side : { KINGSIDE, QUEENSIDE }) {
					if (endSquare == FEN::ROOK_SQUARES[capturedColor][side]) {
						if (castle[capturedColor][side]) {
							removeCastle(capturedColor, side);
						}

						break;
					}
				}
			}
		}
		
		if (piece == ROOK) {
			for (Castle side : { KINGSIDE, QUEENSIDE }) {
				if (startSquare == FEN::ROOK_SQUARES[toMove][side]) {
					if (castle[toMove][side]) {
						removeCastle(toMove, side);
					}

					break;
				}
			}
		} else if (piece == KING) {
			removeCastles(toMove);
		} else if (piece == PAWN && distance(startSquare, endSquare) == 2) {
			enPassant = backwardOne(endSquare, toMove);
			
			zkey ^= zhash.getEnPassant()[getFile(enPassant)];
		}
	} else if (move.isCastle()) {
		if (move == CASTLE_MOVES[toMove][KINGSIDE]) {
			removePiece(toMove == WHITE ? 7 : 63, toMove, ROOK);
			putPiece(toMove == WHITE ? 5 : 61, toMove, ROOK);
		} else {
			removePiece(toMove == WHITE ? 0 : 56, toMove, ROOK);
			putPiece(toMove == WHITE ? 3 : 59, toMove, ROOK);
		}

		removeCastles(toMove);
	} else if (move.isPromotion()) {
		auto [capturedColor, capturedPiece] = findPiece(endSquare, opponent);

		if (capturedPiece != NO_PIECE) {
			removePiece(endSquare, opponent, capturedPiece);

			if (capturedPiece == ROOK) {
				for (Castle side : { KINGSIDE, QUEENSIDE }) {
					if (endSquare == FEN::ROOK_SQUARES[capturedColor][side]) {
						if (castle[capturedColor][side]) {
							removeCastle(capturedColor, side);
						}

						break;
					}
				}
			}
		}
	} else if (move.isEnPassant()) {
		int epPawnSquare = backwardOne(endSquare, toMove);
		removePiece(epPawnSquare, opponent, PAWN);
	}

	if (!move.isPromotion()) {
		putPiece(endSquare, color, piece);
	} else {
		putPiece(endSquare, toMove, move.getPromotion());
	}

	toMove = opponent;
	
	zkey ^= zhash.getToMove();
  
  fullmoveNumber++;
}