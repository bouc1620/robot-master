#include <string>

#include "types.h"
#include "util.h"
#include "move.h"
#include "fen.h"

int Move::getStartSquare() const {
	return (move & 0x0FC0) >> 6;
}

int Move::getEndSquare() const {
	return move & 0x0003F;
}

Piece Move::getPromotion() const {
	if (isPromotion()) {
		return static_cast<Piece>(((move & 0x3000) >> 12) + 1);
	}

	return NO_PIECE;
}

bool Move::isNormal() const {
	return (move & 0xC000) == 0x0000;
}

bool Move::isCastle() const {
	return (move & 0xC000) == 0x4000;
}

bool Move::isPromotion() const {
	return (move & 0xC000) == 0x8000;
}

bool Move::isEnPassant() const {
	return (move & 0xC000) == 0xC000;
}

std::string Move::toString() const {
	int startSquare = getStartSquare();
	int endSquare = getEndSquare();

	std::string result = square_itos(startSquare) + square_itos(endSquare);

	Piece promotion = getPromotion();
	if (promotion != NO_PIECE) {
		result += FEN::PIECE_CHARS[BLACK][promotion];
	}

	return result;
}

bool Move::operator==(const Move& rhs) const {
	return move == rhs.move;
}