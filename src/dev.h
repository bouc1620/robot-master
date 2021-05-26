#ifndef DEV_H
#define DEV_H

#include <string>

#include "bitboard.h"
#include "board.h"
#include "fen.h"

void perftDivide(int, std::string = FEN::START_POS);
void printU64(U64, std::string = "");
int randomSquare();
U64 randomU64();

#endif