#include <initializer_list>

#include "zobrist-hash.h"
#include "bitboard.h"
#include "types.h"

ZobristHash::ZobristHash() :
	x{ SEED },
	pieces{},
	castle{},
	enPassant{},
	toMove{} {
	for (Piece piece : { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING }) {
		for (int square = 0; square < 64; square++) {
			pieces[WHITE][piece][square] = next();
			pieces[BLACK][piece][square] = next();
		}
	}

	for (Castle side : { KINGSIDE, QUEENSIDE }) {
		castle[WHITE][side] = next();
		castle[BLACK][side] = next();
	}

	for (int file = 0; file < 8; file++) {
		enPassant[file] = next();
	}

	toMove = next();
}

const U64(&ZobristHash::getPieces() const)[2][6][64]{
	return pieces;
}

const U64(&ZobristHash::getCastle() const)[2][2]{
	return castle;
}

const U64(&ZobristHash::getEnPassant() const)[8]{
	return enPassant;
}

U64 ZobristHash::getToMove() const {
	return toMove;
}

// xorshift* PRNG by Sebastiano Vigna
// https://github.com/jj1bdx/xorshiftplus-c/blob/master/xorshift64star.c
U64 ZobristHash::next() {
	x ^= x >> 12;
	x ^= x << 25;
	x ^= x >> 27;
	return x * U64(0x2545F4914F6CDD1D);
}