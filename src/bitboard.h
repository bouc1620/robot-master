#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>

typedef uint64_t U64;

const constexpr U64 ZERO = 0;
const constexpr U64 ONE = 1;
const constexpr U64 FULL = ~ZERO;

const constexpr U64 U64_FILE_A = 0x0101010101010101;
const constexpr U64 U64_FILE_B = 0x0202020202020202;
const constexpr U64 U64_FILE_C = 0x0404040404040404;
const constexpr U64 U64_FILE_D = 0x0808080808080808;
const constexpr U64 U64_FILE_E = 0x1010101010101010;
const constexpr U64 U64_FILE_F = 0x2020202020202020;
const constexpr U64 U64_FILE_G = 0x4040404040404040;
const constexpr U64 U64_FILE_H = 0x8080808080808080;

const constexpr U64 U64_RANK_1 = 0x00000000000000FF;
const constexpr U64 U64_RANK_2 = 0x000000000000FF00;
const constexpr U64 U64_RANK_3 = 0x0000000000FF0000;
const constexpr U64 U64_RANK_4 = 0x00000000FF000000;
const constexpr U64 U64_RANK_5 = 0x000000FF00000000;
const constexpr U64 U64_RANK_6 = 0x0000FF0000000000;
const constexpr U64 U64_RANK_7 = 0x00FF000000000000;
const constexpr U64 U64_RANK_8 = 0xFF00000000000000;

#endif