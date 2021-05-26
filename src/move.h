#ifndef MOVE_H
#define MOVE_H

#include <cstdint>
#include <string>
#include <vector>

#include "types.h"

// special | promotion | origin | destination
// 14 - 15 |	12 - 13	 | 6 - 11 |		0 - 5
// 
// special: normal move -> 0, castle -> 1, promotion -> 2, en passant -> 3

class Move {
	uint16_t move;

public:
	constexpr Move(int, int, Special = NORMAL, Piece = NO_PIECE);
	int getStartSquare() const;
	int getEndSquare() const;
	Piece getPromotion() const;
	bool isNormal() const;
	bool isCastle() const;
	bool isPromotion() const;
	bool isEnPassant() const;
	std::string toString() const;
	bool operator==(const Move& rhs) const;
};

constexpr Move::Move(int startSquare, int endSquare, Special special, Piece promotion) :
	move{} {
	move = (startSquare << 6) | endSquare;

	switch (special)
	{
	case NORMAL: break;
	case CASTLE: move |= 0x4000; break;
	case PROMOTION: move |= 0x8000 | ((promotion - 1) << 12); break;
	case EN_PASSANT: move |= 0xC000; break;
	default: break;
	}
}

const constexpr Move CASTLE_MOVES[2][2] =
{
	{ Move( 4,	6, CASTLE), Move( 4,	2, CASTLE) },
	{ Move(60, 62, CASTLE), Move(60, 58, CASTLE) }
};

typedef std::vector<Move> MoveList;

#endif