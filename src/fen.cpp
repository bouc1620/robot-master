#include <initializer_list>
#include <algorithm>
#include <sstream>
#include <string>
#include <regex>

#include "bitboard.h"
#include "attack.h"
#include "board.h"
#include "util.h"
#include "fen.h"

namespace {
	bool validateCastle(U64(&pieces)[2][6], std::string castle) {
		if (castle != "-") {
			for (Castle side : { KINGSIDE, QUEENSIDE }) {
				for (Color color : { WHITE, BLACK }) {
					int count = std::count(castle.begin(), castle.end(), FEN::CASTLE_CHARS[color][side]);

					if (count > 1) {
						return false;
					}

					if (count == 1) {
						if (!(pieces[color][KING] & (ONE << (color == WHITE ? 0 : 60)))) {
							return false;
						}

						if (!(pieces[color][ROOK] & (ONE << FEN::ROOK_SQUARES[color][side]))) {
							return false;
						}
					}
				}
			}
		}

		return true;
	}

	bool validateEnPassant(U64(&pieces)[2][6], U64(&allPieces)[2], std::string ep, Color toMove) {
		static const int EP_FILES[2] =
		{	5,	2 };

		if (ep != "-") {
			int square = square_stoi(ep);
			int rank = getRank(square);

			if (rank != EP_FILES[toMove]) {
				return false;
			}

			for (Color color : { WHITE, BLACK }) {
				if (allPieces[color] & (ONE << square)) {
					return false;
				}
			}

			int pawnSquare = toMove == WHITE ? square - 8 : square + 8;

			if (!(pieces[opposite(toMove)][PAWN] & (ONE << pawnSquare))) {
				return false;
			}

			if (!(attack::pawn(square, opposite(toMove)) & pieces[toMove][PAWN])) {
				return false;
			}
		}

		return true;
	}

	bool kingInCheck(U64(&pieces)[2][6], U64(&allPieces)[2], Color toMove) {
		Color opponent = opposite(toMove);

		U64 king = pieces[toMove][KING];
		int kingSquare = bitscanForward(king);

		if (attack::pawn(kingSquare, toMove) & pieces[opponent][PAWN]) {
			return true;
		}

		if (attack::knight(kingSquare) & pieces[opponent][KNIGHT]) {
			return true;
		}

		U64 blockers = allPieces[WHITE] | allPieces[BLACK];

		for (Piece slider : { BISHOP, ROOK, QUEEN }) {
			if (attack::slider(kingSquare, blockers, slider) & pieces[opponent][slider]) {
				return true;
			}
		}

		if (attack::king(kingSquare) & pieces[opponent][KING]) {
			return true;
		}

		return false;
	}
}

bool FEN::validateFEN(std::string FEN) {
	static const std::string FEN_REGEX = "\\s*([pnbrqkNBRQK1-8]+\\/)([pnbrqkPNBRQK1-8]+\\/){6}([nbrq"
		"kPNBRQK1-8]+)\\s+[wb-]\\s+(([KQkq]{1,4})|(-))\\s+(([a-h][36])|(-))\\s+\\d+\\s+\\d+\\s*";

	if (!std::regex_match(FEN, std::regex(FEN_REGEX))) {
		return false;
	}

	std::stringstream ss(FEN);

	std::string position,
		toMove,
		castle,
		ep;

	ss >> position;

	U64 pieces[2][6]{};
	U64 allPieces[2]{};

	int i = 0, c = 0, square = 56;
	for (; i < position.length() && square >= 0 && square <= 64; i++) {
		if (position[i] == '/') {
			if (c != 8) {
				return false;
			}

			c = 0;
			square += 2 * -8;
		} else if (isdigit(position[i])) {
			int n = position[i] - '0';

			c += n;
			square += n;
		} else {
			for (Piece piece : { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING }) {
				for (Color color : { WHITE, BLACK }) {
					if (position[i] == PIECE_CHARS[color][piece]) {
						pieces[color][piece] |= ONE << square;
						allPieces[color] |= ONE << square;
						break;
					}
				}
			}

			c++;
			square++;
		}
	}
	
	if (i != position.length() || c != 8) {
		return false;
	}

	for (Color color : { WHITE, BLACK }) {
		if (popCount(pieces[color][KING]) != 1) {
			return false;
		}
	}

	ss >> toMove;

	Color toMove_ = toMove[0] == COLOR_CHARS[0] ? WHITE : BLACK;

	ss >> castle;

	if (!validateCastle(pieces, castle)) {
		return false;
	}

	ss >> ep;

	if (!validateEnPassant(pieces, allPieces, ep, toMove_)) {
		return false;
	}

	if (kingInCheck(pieces, allPieces, opposite(toMove_))) {
		return false;
	}

	return true;
}

std::string FEN::getFEN(const Board& board) {
	// TODO: generate FEN position based on board attributes

	return "";
}