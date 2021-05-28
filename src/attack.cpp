#include "bitboard.h"
#include "attack.h"
#include "types.h"
#include "util.h"

namespace attack {
	U64 pawn(int square, Color color) {
		U64 pawn = ONE << square;

		if (color == WHITE) {
			return ((pawn & ~U64_FILE_A) << 7) | ((pawn & ~U64_FILE_H) << 9);
		} else {
			return ((pawn & ~U64_FILE_A) >> 9) | ((pawn & ~U64_FILE_H) >> 7);
		}
	}

	U64 pawnKingside(U64 pieces, Color color) {
		return color == WHITE ? (pieces & ~U64_FILE_H) << 9 : (pieces & ~U64_FILE_H) >> 7;
	}

	U64 pawnQueenside(U64 pieces, Color color) {
		return color == WHITE ? (pieces & ~U64_FILE_A) << 7 : (pieces & ~U64_FILE_A) >> 9;
	}

	U64 knight(int square) {
		return details::KNIGHT_LOOKUP[square];
	}

	U64 bishop(int square, U64 blockers) {
		int diag = getDiag(square);
		int antidiag = getAntidiag(square);
		
		int file = getFile(square);

		U64 mask = details::SLIDER_MASKS[file];

		U64 diagAttacks;
		if (diag <= 1 || diag >= 13) {
			diagAttacks = U64_DIAGS[diag];
		} else {
			U64 diagBlockers = ((blockers & U64_DIAGS[diag]) * U64_FILE_A) >> 56;

			diagAttacks = details::SLIDER_LOOKUP[file][diagBlockers & mask];
			diagAttacks = (diagAttacks * U64_FILE_A) & U64_DIAGS[diag];
		}

		U64 antidiagAttacks;
		if (antidiag <= 1 || antidiag >= 13) {
			antidiagAttacks = U64_ADIAGS[antidiag];
		} else {
			U64 antidiagBlockers = ((blockers & U64_ADIAGS[antidiag]) * U64_FILE_A) >> 56;

			antidiagAttacks = details::SLIDER_LOOKUP[file][antidiagBlockers & mask];
			antidiagAttacks = (antidiagAttacks * U64_FILE_A) & U64_ADIAGS[antidiag];
		}

		return diagAttacks | antidiagAttacks;
	}

	U64 rook(int square, U64 blockers) {
		int rank = getRank(square);
		int file = getFile(square);

		U64 rankBlockers = (blockers >> (8 * rank)) & U64_RANK_1;

		U64 rankMask = details::SLIDER_MASKS[file];

		U64 rankAttacks = details::SLIDER_LOOKUP[file][rankBlockers & rankMask];
		rankAttacks <<= 8 * rank;
		
		U64 fileBlockers = (((blockers >> file) & U64_FILE_A) * U64_DIAG_A) >> 56;

		U64 fileMask = details::SLIDER_MASKS[7 - rank];
		
		U64 fileAttacks = details::SLIDER_LOOKUP[7 - rank][fileBlockers & fileMask];
		fileAttacks = (((fileAttacks * U64_DIAG_A) >> 7) & U64_FILE_A) << file;

		return rankAttacks | fileAttacks;
	}

	U64 queen(int square, U64 blockers) {
		return rook(square, blockers) | bishop(square, blockers);
	}

	U64 slider(int square, U64 blockers, Piece type) {
		switch (type) {
		case BISHOP: return bishop(square, blockers);
		case ROOK: return rook(square, blockers);
		case QUEEN: return queen(square, blockers);
		default: return ZERO;
		}
	}

	U64 king(int square) {
		return details::KING_LOOKUP[square];
	}
}