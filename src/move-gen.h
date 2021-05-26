#ifndef MOVE_GEN_H
#define MOVE_GEN_H

#include <optional>
#include <array>
#include <map>

#include "board.h"
#include "move.h"

class MoveGen {
	const Board& board;
  unsigned int fullmoveNumber;
	std::optional<MoveList> legalMoves;
	std::map<int, U64> absolutePins;
	U64 checkMask;
	int checkCount;

public:
	MoveGen() = delete;
	MoveGen(const Board&);
	bool isCheckmate();
	const MoveList& getLegalMoves();

private:
	void clear();
	MoveList getKingMoves();
	MoveList getPawnMoves() const;
	MoveList getKnightMoves() const;
	MoveList getSliderMoves(Piece) const;
	void findAbsolutePins();
	U64 findRay(int, int) const;
};

#endif