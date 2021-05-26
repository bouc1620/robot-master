#ifndef FEN_H
#define FEN_H

#include <string>

class Board;

namespace FEN {
	const inline std::string START_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

	const inline char PIECE_CHARS[2][6] =
	{
		{ 'P', 'N', 'B', 'R', 'Q', 'K' },
		{ 'p', 'n', 'b', 'r', 'q', 'k' }
	};

	const inline char CASTLE_CHARS[2][2] =
	{
		{ 'K', 'Q' },
		{ 'k', 'q' }
	};

	const inline char COLOR_CHARS[2] =
	{ 'w', 'b' };

	const inline int ROOK_SQUARES[2][2] =
	{
		{	7,	0 },
		{ 63, 56 }
	};

	bool validateFEN(std::string);
	std::string getFEN(const Board&);
}

#endif