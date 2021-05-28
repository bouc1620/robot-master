#include <functional>
#include <optional>

#include "move-gen.h"
#include "bitboard.h"
#include "attack.h"
#include "board.h"
#include "util.h"
#include "move.h"

MoveGen::MoveGen(const Board& board) :
	board{ board },
  fullmoveNumber{ board.getFullmoveNumber() },
	legalMoves{},
	absolutePins{},
	checkMask{ FULL },
	checkCount{ 0 } {
}

void MoveGen::clear() {
	fullmoveNumber = board.getFullmoveNumber();
	legalMoves.reset();
	absolutePins.clear();
	checkMask = FULL;
	checkCount = 0;
}

bool MoveGen::isCheckmate() {
	return getLegalMoves().empty() && checkCount > 0;
}

const MoveList& MoveGen::getLegalMoves() {
	if (board.getFullmoveNumber() != fullmoveNumber) {
		clear();
	} else if (legalMoves) {
		return legalMoves.value();
	}

	legalMoves = MoveList{};

	findKingMoves();

	if (checkCount >= 2) {
		return legalMoves.value();
	}

	findAbsolutePins();

	findPawnMoves();

	findKnightMoves();

	for (Piece slider : { BISHOP, ROOK, QUEEN }) {
		findSliderMoves(slider);
	}

	return legalMoves.value();
}

void MoveGen::findKingMoves() {
	const std::function<U64(U64, U64, Color)> opponentAttacks[] =
	{
		[this](U64 king, U64 blockers, Color opponent) -> U64 {
			U64 controlled = ZERO;

			for (Piece slider : { BISHOP, ROOK, QUEEN }) {
				U64 sliders = board.getPieces()[opponent][slider];

				while (sliders) {
					int sliderSquare = popLSB(sliders);

					U64 attacks = attack::slider(sliderSquare, blockers, slider);

					controlled |= attacks;

					if (king & attacks) {
						checkCount++;
						checkMask = getRay(bitscanForward(king), sliderSquare);
					}
				}
			}

			return controlled;
		},

		[this](U64 king, U64 blockers, Color opponent) -> U64 {
			U64 controlled = ZERO;

			U64 knights = board.getPieces()[opponent][KNIGHT];

			while (knights) {
				int knightSquare = popLSB(knights);

				U64 attacks = attack::knight(knightSquare);

				controlled |= attacks;

				if (king & attacks) {
					checkCount++;
					checkMask = ONE << knightSquare;
				}
			}

			return controlled;
		},

		[this](U64 king, U64 blockers, Color opponent) -> U64 {
			U64 pawns = board.getPieces()[opponent][PAWN];

			U64 attacksKingside = attack::pawnKingside(pawns, opponent);
			U64 attacksQueenside = attack::pawnQueenside(pawns, opponent);

			if (king & attacksKingside) {
				checkCount++;
				checkMask = attack::pawnQueenside(king, board.getToMove());
			} else if (king & attacksQueenside) {
				checkCount++;
				checkMask = attack::pawnKingside(king, board.getToMove());
			}

			return attacksKingside | attacksQueenside;
		},

		[this](U64 king, U64 blockers, Color opponent) -> U64 {
			U64 opponentKing = board.getPieces()[opponent][KING];
			int opponentKingSquare = bitscanForward(opponentKing);

			return attack::king(opponentKingSquare);
		}
	};

	Color opponent = opposite(board.getToMove());

	U64 king = board.getPieces()[board.getToMove()][KING];
	int kingSquare = bitscanForward(king);

	U64 blockers = (board.getAllPieces()[WHITE] | board.getAllPieces()[BLACK]) & ~king;

	U64 attacks = attack::king(kingSquare);
	attacks &= ~(board.getAllPieces()[board.getToMove()]);

	U64 controlled = ZERO;
	for (auto& check : opponentAttacks) {
		controlled |= check(king, blockers, opponent);

		if (checkCount > 1 && !(attacks & ~controlled)) {
			return;
		}
	}
	
	attacks &= ~controlled;

	while (attacks) {
		int endSquare = popLSB(attacks);
		legalMoves.value().push_back(Move(kingSquare, endSquare));
	}

	static const std::array<std::array<U64, 2>, 2> CASTLE_ATTACKS_MASK =
	{ {
		{ 0x0000000000000060, 0x000000000000000C },
		{ 0x6000000000000000, 0x0C00000000000000 }
	} };

	static const std::array<std::array<U64, 2>, 2> CASTLE_BLOCKERS_MASK =
	{ {
		{ 0x0000000000000060, 0x000000000000000E },
		{ 0x6000000000000000, 0x0E00000000000000 }
	} };

	if (checkCount == 0) {
		std::array<U64, 2> attacksMask = CASTLE_ATTACKS_MASK[board.getToMove()];
		std::array<U64, 2> blockersMask = CASTLE_BLOCKERS_MASK[board.getToMove()];

		for (Castle side : { KINGSIDE, QUEENSIDE }) {
			if (board.getCastle()[board.getToMove()][side]) {
				attacksMask[side] &= ~controlled;
				blockersMask[side] &= ~blockers;

				if (attacksMask[side] == CASTLE_ATTACKS_MASK[board.getToMove()][side] &&
					blockersMask[side] == CASTLE_BLOCKERS_MASK[board.getToMove()][side]) {

					legalMoves.value().push_back(CASTLE_MOVES[board.getToMove()][side]);
				}
			}
		}
	}
}

void MoveGen::findPawnMoves() {
	Color opponent = opposite(board.getToMove());

	U64 pawns = board.getPieces()[board.getToMove()][PAWN];

	while (pawns) {
		int pawnSquare = popLSB(pawns);
		U64 pawn = ONE << pawnSquare;

		U64 attacks = attack::pawn(pawnSquare, board.getToMove());

		U64 blockers = board.getAllPieces()[WHITE] | board.getAllPieces()[BLACK];

		U64 pushes = (ONE << forwardOne(pawnSquare, board.getToMove())) & ~blockers;

		if (pushes && (board.getToMove() == WHITE ? pawn & U64_RANK_2 : pawn & U64_RANK_7)) {
			pushes |= (ONE << forwardTwo(pawnSquare, board.getToMove())) & ~blockers;
		}

		std::map<int, U64>::const_iterator pin = absolutePins.find(pawnSquare);
		if (pin != absolutePins.cend()) {
			attacks &= pin->second;
			pushes &= pin->second;
		}
		
		U64 enPassant = ZERO;
		if (board.getEnPassant() != NO_SQUARE && attacks & (ONE << board.getEnPassant())) {
			int epPawnSquare = backwardOne(board.getEnPassant(), board.getToMove());

			if ((checkMask == FULL || checkMask == (ONE << epPawnSquare)) &&
				absolutePins.count(epPawnSquare) == 0) {
				
				enPassant = ONE << board.getEnPassant();
			}
		}

		attacks &= board.getAllPieces()[opponent] & checkMask;
		pushes &= checkMask;

		U64 promotions = (attacks | pushes) & (board.getToMove() == WHITE ? U64_RANK_8 : U64_RANK_1);

		attacks &= ~promotions;
		pushes &= ~promotions;

		while (attacks) {
			int endSquare = popLSB(attacks);
			legalMoves.value().push_back(Move(pawnSquare, endSquare));
		}

		while (pushes) {
			int endSquare = popLSB(pushes);
			legalMoves.value().push_back(Move(pawnSquare, endSquare));
		}

		if (enPassant) {
			int endSquare = bitscanForward(enPassant);
			legalMoves.value().push_back(Move(pawnSquare, endSquare, EN_PASSANT));
		}

		while (promotions) {
			int endSquare = popLSB(promotions);

			for (Piece promotion : { KNIGHT, BISHOP, ROOK, QUEEN }) {
				legalMoves.value().push_back(Move(pawnSquare, endSquare, PROMOTION, promotion));
			}
		}
	}
}

void MoveGen::findKnightMoves() {
	Color opponent = opposite(board.getToMove());

	U64 knights = board.getPieces()[board.getToMove()][KNIGHT];

	while (knights) {
		int knightSquare = popLSB(knights);

		if (absolutePins.count(knightSquare) != 0) {
			continue;
		}

		U64 attacks = attack::knight(knightSquare);

		attacks &= ~(board.getAllPieces()[board.getToMove()]) & checkMask;

		while (attacks) {
			int endSquare = popLSB(attacks);
			legalMoves.value().push_back(Move(knightSquare, endSquare));
		}
	}
}

void MoveGen::findSliderMoves(Piece piece) {
	Color opponent = opposite(board.getToMove());

	U64 sliders = board.getPieces()[board.getToMove()][piece];

	while (sliders) {
		int sliderSquare = popLSB(sliders);

		U64 blockers = board.getAllPieces()[WHITE] | board.getAllPieces()[BLACK];

		U64 attacks = attack::slider(sliderSquare, blockers, piece);

		attacks &= ~(board.getAllPieces()[board.getToMove()]) & checkMask;

		std::map<int, U64>::const_iterator pin = absolutePins.find(sliderSquare);
		if (pin != absolutePins.cend()) {
			attacks &= pin->second;
		}

		while (attacks) {
			int endSquare = popLSB(attacks);
			legalMoves.value().push_back(Move(sliderSquare, endSquare));
		}
	}
}

void MoveGen::findAbsolutePins() {
	Color opponent = opposite(board.getToMove());

	U64 king = board.getPieces()[board.getToMove()][KING];
	int kingSquare = bitscanForward(king);

	U64 kingT = attack::rook(kingSquare, board.getAllPieces()[opponent]);

	U64 rooksAndQueens = board.getPieces()[opponent][ROOK] | board.getPieces()[opponent][QUEEN];
	
	if (kingT & rooksAndQueens) {
		const std::array<U64, 4>& raysT = attack::ROOK_RAYS[kingSquare];

		for (RookDirection dir : { NORTH, SOUTH, WEST, EAST }) {
			U64 attackRay = kingT & raysT[dir];

			if (attackRay & rooksAndQueens) {
				U64 shielders = attackRay & board.getAllPieces()[board.getToMove()];

				if (popCount(shielders) == 1) {
					int shielderSquare = bitscanForward(shielders);

					absolutePins.insert({ shielderSquare, attackRay });
				}
			}
		}
	}

	U64 kingX = attack::bishop(kingSquare, board.getAllPieces()[opponent]);

	U64 bishopsAndQueens = board.getPieces()[opponent][BISHOP] | board.getPieces()[opponent][QUEEN];

	if (kingX & bishopsAndQueens) {
		const std::array<U64, 4>& raysX = attack::BISHOP_RAYS[kingSquare];

		for (BishopDirection dir : { NORTH_WEST, SOUTH_EAST, SOUTH_WEST, NORTH_EAST }) {
			U64 attackRay = kingX & raysX[dir];

			if (attackRay & bishopsAndQueens) {
				U64 shielders = attackRay & board.getAllPieces()[board.getToMove()];

				if (popCount(shielders) == 1) {
					int shielderSquare = bitscanForward(shielders);

					absolutePins.insert({ shielderSquare, attackRay });
				}
			}
		}
	}

	if (board.getEnPassant() != NO_SQUARE) {
		int epPawnSquare = backwardOne(board.getEnPassant(), board.getToMove());

		if (getRank(kingSquare) == getRank(epPawnSquare)) {
			U64 blockers = board.getAllPieces()[opponent] & ~(ONE << epPawnSquare);

			kingT = attack::rook(kingSquare, blockers);

			const std::array<U64, 4>& raysT = attack::ROOK_RAYS[kingSquare];

			for (RookDirection dir : { WEST, EAST }) {
				U64 attackRay = kingT & raysT[dir];

				if ((attackRay & rooksAndQueens) && (attackRay & (ONE << epPawnSquare))) {
					U64 shielder = attackRay & board.getAllPieces()[board.getToMove()];

					if (popCount(shielder) < 2) {
						absolutePins.insert({ epPawnSquare, attackRay });
					}

					break;
				}
			}
		}
	}
}

U64 MoveGen::getRay(int startSquare, int endSquare) const {
	U64 end = ONE << endSquare;

	U64 startT = attack::rook(startSquare, end);

	if (startT & end) {
		const std::array<U64, 4>& raysT = attack::ROOK_RAYS[startSquare];

		for (RookDirection dir : { NORTH, SOUTH, WEST, EAST }) {
			U64 ray = raysT[dir] & startT;

			if (ray & end) {
				return ray;
			}
		}
	}
	
	U64 startX = attack::bishop(startSquare, end);

	if (startX & end) {
		const std::array<U64, 4>& raysX = attack::BISHOP_RAYS[startSquare];

		for (BishopDirection dir : { NORTH_WEST, SOUTH_EAST, SOUTH_WEST, NORTH_EAST }) {
			U64 ray = raysX[dir] & startX;

			if (ray & end) {
				return ray;
			}
		}
	}

	return FULL;
}