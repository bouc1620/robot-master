#ifndef ZOBRIST_HASH_H
#define ZOBRIST_HASH_H

#include "bitboard.h"

class ZobristHash {
	static const inline U64 SEED = 0xDDEF04141420EF9F;
	U64 x;
	U64 pieces[2][6][64];
	U64 castle[2][2];
	U64 enPassant[8];
	U64 toMove;

public:
	ZobristHash();
	const U64(&getPieces() const)[2][6][64];
	const U64(&getCastle() const)[2][2];
	const U64(&getEnPassant() const)[8];
	U64 getToMove() const;

private:
	U64 next();
};

#endif
