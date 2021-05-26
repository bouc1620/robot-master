#ifndef TYPES_H
#define TYPES_H

const inline int NO_SQUARE = -1;

enum Color {
	WHITE,
	BLACK,
	NO_COLOR
};

enum Piece {
	PAWN,
	KNIGHT,
	BISHOP,
	ROOK,
	QUEEN,
	KING,
	NO_PIECE
};

enum Castle {
	KINGSIDE,
	QUEENSIDE,
	NO_SIDE
};

enum Special {
	NORMAL,
	CASTLE,
	PROMOTION,
	EN_PASSANT
};

enum RookDirection {
	NORTH,
	SOUTH,
	WEST,
	EAST
};

enum BishopDirection {
	NORTH_WEST,
	SOUTH_EAST,
	SOUTH_WEST,
	NORTH_EAST
};

#endif