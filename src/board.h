#ifndef BOARD_H
#define BOARD_H

#include <string>

#include "zobrist-hash.h"
#include "bitboard.h"
#include "types.h"
#include "move.h"
#include "fen.h"

class Board {
	U64 pieces[2][6];
	U64 allPieces[2];
	Color toMove;
	bool castle[2][2];
	int enPassant;
	unsigned int halfmoveClock;
	unsigned int fullmoveNumber;
	ZobristHash zhash;
	U64 zkey;

public:
	Board(std::string = FEN::START_POS);
	std::pair<Color, Piece> findPiece(int, Color = NO_COLOR) const;
	void doMove(Move);
	const U64(&getPieces() const)[2][6];
	const U64(&getAllPieces() const)[2];
	Color getToMove() const;
	const bool(&getCastle() const)[2][2];
	int getEnPassant() const;
	unsigned int getHalfmoveClock() const;
	unsigned int getFullmoveNumber() const;
	U64 getZkey() const;
	std::string toString() const;

private:
	void setup(std::string);
	void removeCastle(Color, Castle);
	void removeCastles(Color);
	void putPiece(int, Color, Piece);
	void removePiece(int, Color, Piece);
};

#endif