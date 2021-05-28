#ifndef UTIL_H
#define UTIL_H

#include <algorithm>
#include <string>

#include "bitboard.h"
#include "types.h"

constexpr int getFile(int square) {
	return square % 8;
}

constexpr int getRank(int square) {
	return square / 8;
}

inline int getDiag(int square) {
	return 7 - (getRank(square) - getFile(square));
}

inline int getAntidiag(int square) {
	return getRank(square) + getFile(square);
}

constexpr int getSquare(int file, int rank) {
	return file + rank * 8;
}

inline int square_stoi(std::string square) {
	return getSquare(square[0] - 'a', square[1] - '1');
}

inline int backwardOne(int square, Color color) {
	return color == WHITE ? square - 8 : square + 8;
}

inline int forwardOne(int square, Color color) {
	return color == WHITE ? square + 8 : square - 8;
}

inline int forwardTwo(int square, Color color) {
	return color == WHITE ? square + 16 : square - 16;
}

inline std::string square_itos(int square) {
	char file = getFile(square) + 'a';
	char rank = getRank(square) + '1';
	return std::string{ file, rank };
}

inline int distance(int square1, int square2) {
	int fileDistance = std::abs(getFile(square1) - getFile(square2));
	int rankDistance = std::abs(getRank(square1) - getRank(square2));
	return std::max(fileDistance, rankDistance);
}

inline Color opposite(Color color) {
	return color == WHITE ? BLACK : WHITE;
}

constexpr int popCount(U64 board) {
	return __builtin_popcountll(board);
}

constexpr int popLSB(U64& board) {
	int square = __builtin_ctzll(board);
	board &= board - 1;

	return square;
}

constexpr int bitscanForward(U64 board) {
	return __builtin_ctzll(board);
}

#endif