#ifndef ATTACK_H
#define ATTACK_H

#include <array>

#include "bitboard.h"
#include "types.h"
#include "util.h"

namespace attack {	
	namespace details {
		constexpr U64 blockersPermutation(int iter, U64 mask) {
			U64 blockers = ZERO;

			while (iter) {
				if (iter & 1) {
					int square = bitscanForward(mask);
					blockers |= ONE << square;
				}

				iter >>= 1;
				mask &= mask - 1;
			}

			return blockers;
		}

		const constexpr auto SLIDER_MASKS = []() {
			std::array<U64, 8> masks{};

			for (int square = 0; square < 8; square++) {
				U64 mask = ZERO;
				int rank = 0;
				int file = getFile(square);

				for (int file_ = 1; file_ < 7; file_++) {
					if (file_ != file) {
						mask |= ONE << getSquare(file_, rank);
					}
				}

				masks[square] = mask;
			}

			return masks;
		}();

		const constexpr auto SLIDER_LOOKUP = []() {
			std::array<std::array<U64, 128>, 8> lookup{};

			for (int square = 0; square < 8; square++) {
				U64 mask = SLIDER_MASKS[square];
				int permutationCount = 1 << popCount(mask);

				for (int i = 0; i < permutationCount; i++) {
					U64 blockers = blockersPermutation(i, mask);
					U64 attacks = ZERO;
					int rank = 0;
					int file = getFile(square), file_ = 0;

					for (int file_ = file + 1; file_ <= 7; file_++) {
						attacks |= ONE << file_;
						if (blockers & (ONE << file_)) {
							break;
						}
					}

					for (int file_ = file - 1; file_ >= 0; file_--) {
						attacks |= ONE << file_;
						if (blockers & (ONE << file_)) {
							break;
						}
					}

					lookup[square][blockers] = attacks;
				}
			}

			return lookup;
		}();

		const constexpr auto KNIGHT_LOOKUP = []() {
			std::array<U64, 64> lookup{};

			for (int square = 0; square < 64; square++) {
				U64 knight = ONE << square;

				U64 left1 = (knight >> 1) & ~U64_FILE_H;
				U64 left2 = (knight >> 2) & ~(U64_FILE_G | U64_FILE_H);
				U64 right1 = (knight << 1) & ~U64_FILE_A;
				U64 right2 = (knight << 2) & ~(U64_FILE_A | U64_FILE_B);
				U64 half1 = left1 | right1;
				U64 half2 = left2 | right2;

				lookup[square] = (half1 << 16) | (half1 >> 16) | (half2 << 8) | (half2 >> 8);
			}

			return lookup;
		}();

		const constexpr auto KING_LOOKUP = []() {
			std::array<U64, 64> lookup{};

			for (int square = 0; square < 64; square++) {
				U64 king = ONE << square;

				U64 attacks = ((king >> 1) & ~U64_FILE_H) | ((king << 1) & ~U64_FILE_A);
				king |= attacks;
				attacks |= (king << 8) | (king >> 8);

				lookup[square] = attacks;
			}

			return lookup;
		}();
	}

	const constexpr auto ROOK_RAYS = []() {
		std::array<std::array<U64, 4>, 64> rays{};

		for (int square = 0; square < 64; square++) {
			U64 ray1 = ZERO;
			U64 ray2 = ZERO;
			int file = getFile(square), file_ = 0;
			int rank = getRank(square), rank_ = 0;

			std::array<U64, 4>::iterator rays_it = rays[square].begin();

			for (rank_ = rank + 1; rank_ <= 7; rank_++) {
				ray1 |= ONE << getSquare(file, rank_);
			}

			for (rank_ = rank - 1; rank_ >= 0; rank_--) {
				ray2 |= ONE << getSquare(file, rank_);
			}

			*rays_it++ = ray1;
			*rays_it++ = ray2;

			for (file_ = file - 1, ray1 = ZERO; file_ >= 0; file_--) {
				ray1 |= ONE << getSquare(file_, rank);
			}

			for (file_ = file + 1, ray2 = ZERO; file_ <= 7; file_++) {
				ray2 |= ONE << getSquare(file_, rank);
			}

			*rays_it++ = ray1;
			*rays_it++ = ray2;
		}

		return rays;
	}();

	const constexpr auto BISHOP_RAYS = []() {
		std::array<std::array<U64, 4>, 64> rays{};

		for (int square = 0; square < 64; square++) {
			U64 ray1 = ZERO;
			U64 ray2 = ZERO;
			int file = getFile(square), file_ = 0;
			int rank = getRank(square), rank_ = 0;

			std::array<U64, 4>::iterator rays_it = rays[square].begin();

			for (rank_ = rank + 1, file_ = file - 1, ray1 = ZERO; rank_ <= 7 && file_ >= 0; rank_++, file_--) {
				ray1 |= ONE << getSquare(file_, rank_);
			}

			for (rank_ = rank - 1, file_ = file + 1, ray2 = ZERO; rank_ >= 0 && file_ <= 7; rank_--, file_++) {
				ray2 |= ONE << getSquare(file_, rank_);
			}

			*rays_it++ = ray1;
			*rays_it++ = ray2;

			for (rank_ = rank - 1, file_ = file - 1, ray1 = ZERO; rank_ >= 0 && file_ >= 0; rank_--, file_--) {
				ray1 |= ONE << getSquare(file_, rank_);
			}

			for (rank_ = rank + 1, file_ = file + 1, ray2 = ZERO; rank_ <= 7 && file_ <= 7; rank_++, file_++) {
				ray2 |= ONE << getSquare(file_, rank_);
			}

			*rays_it++ = ray1;
			*rays_it++ = ray2;
		}

		return rays;
	}();
	
	U64 pawn(int, Color);
	U64 pawnKingside(U64, Color);
	U64 pawnQueenside(U64, Color);
	U64 knight(int);
	U64 bishop(int, U64);
	U64 rook(int, U64);
	U64 queen(int, U64);
	U64 slider(int, U64, Piece);
	U64 king(int);
}

#endif