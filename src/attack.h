#ifndef ATTACK_H
#define ATTACK_H

#include <array>

#include "bitboard.h"
#include "types.h"
#include "util.h"

namespace attack {
	const constexpr auto ROOK_MASKS = []() {
		std::array<U64, 64> masks{};

		for (int square = 0; square < 64; square++) {
			U64 mask = ZERO;
			int rank = getRank(square);
			int file = getFile(square);

			for (int rank_ = 1; rank_ < 7; rank_++) {
				if (rank_ != rank) {
					mask |= ONE << getSquare(file, rank_);
				}
			}

			for (int file_ = 1; file_ < 7; file_++) {
				if (file_ != file) {
					mask |= ONE << getSquare(file_, rank);
				}
			}

			masks[square] = mask;
		}

		return masks;
	}();

	const constexpr auto BISHOP_MASKS = []() {
		std::array<U64, 64> masks{};

		for (int square = 0; square < 64; square++) {
			U64 mask = ZERO;
			int rank = getRank(square), rank_ = 0;
			int file = getFile(square), file_ = 0;

			for (rank_ = rank + 1, file_ = file + 1; rank_ < 7 && file_ < 7; rank_++, file_++) {
				mask |= ONE << getSquare(file_, rank_);
			}

			for (rank_ = rank - 1, file_ = file + 1; rank_ > 0 && file_ < 7; rank_--, file_++) {
				mask |= ONE << getSquare(file_, rank_);
			}

			for (rank_ = rank - 1, file_ = file - 1; rank_ > 0 && file_ > 0; rank_--, file_--) {
				mask |= ONE << getSquare(file_, rank_);
			}

			for (rank_ = rank + 1, file_ = file - 1; rank_ < 7 && file_ > 0; rank_++, file_--) {
				mask |= ONE << getSquare(file_, rank_);
			}

			masks[square] = mask;
		}

		return masks;
	}();

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

		constexpr U64 transform(U64 blockers, U64 magic, int shift) {
			return (blockers * magic) >> shift;
		}

		const constexpr U64 ROOK_MAGICS[64] =
		{
			0x0080008020400012, 0x0440002000100048, 0x0A00104200200880, 0x0880100208000480, 0x0100021004080100, 0x4300080611000400, 0x0400020800E41001, 0x8200040200285081,
			0x0002800040008360, 0x0810402010004000, 0x5002802000801000, 0x01260018120040E0, 0x4000808008000400, 0x0122000201104804, 0x0001000100040200, 0x0014800100004080,
			0x6180084008200241, 0x2020008040002080, 0x0000808010002000, 0x0130010008201102, 0x0002828008000400, 0x0000080120041040, 0x2001808001000200, 0x0208020001006884,
			0x2240822180004000, 0x0401008100204000, 0x0110001080200084, 0x0068090100201003, 0x0201000500080290, 0x8811000300080400, 0x0000010400100208, 0x0800208600010064,
			0x00400040818010A0, 0x0000806002804001, 0x4018100880802000, 0x2004081001002100, 0x4803000801000610, 0x0000201088010440, 0x102100140D003200, 0x08000B5082000423,
			0x1004883240008000, 0x0030004020004008, 0x1000200041010010, 0x0802100100210008, 0x1000080100910004, 0x1A42000804020010, 0x2240020001008080, 0x00000D004882002C,
			0x48FFFE99FECFAA00, 0x48FFFE99FECFAA00, 0x497FFFADFF9C2E00, 0x613FFFDDFFCE9200, 0xFFFFFFE9FFE7CE00, 0xFFFFFFF5FFF3E600, 0x0003FF95E5E6A4C0, 0x510FFFF5F63C96A0,
			0xEBFFFFB9FF9FC526, 0x61FFFEDDFEEDAEAE, 0x53BFFFEDFFDEB1A2, 0x127FFFB9FFDFB5F6, 0x411FFFDDFFDBF4D6, 0x20020048092C102E, 0x0003FFEF27EEBE74, 0x7645FFFECBFEA79E
		};

		const constexpr U64 BISHOP_MAGICS[64] =
		{
			0xFFEDF9FD7CFCFFFF, 0xFC0962854A77F576, 0x0408860402209001, 0x00109086000008B1, 0x2201104009400D10, 0x0202080208701508, 0xFC0A66C64A7EF576, 0x7FFDFDFCBD79FFFF,
			0xFC0846A64A34FFF6, 0xFC087A874A3CF7F6, 0x2000040404004480, 0x0001080611400412, 0x0010040420800A14, 0x08020E2A10402006, 0xFC0864AE59B4FF76, 0x3C0860AF4B35FF76,
			0x73C01AF56CF4CFFB, 0x41A01CFAD64AAFFC, 0x1401100206020200, 0x4028008412102000, 0x0410100202101400, 0x0001002A01008200, 0x7C0C028F5B34FF76, 0xFC0A028E5AB4DF76,
			0x0202202810041000, 0x088220480848008E, 0x4800880110480020, 0x0010040080440008, 0x102084004A020201, 0x0010920001004A28, 0x4140A70084040200, 0x0640420000820102,
			0x0410100440080800, 0x0070842000040800, 0x8400603800040800, 0x0000040401080120, 0x2820040401104102, 0x0110810100061000, 0x005041004C220240, 0x000104002041010A,
			0xDCEFD9B54BFCC09F, 0xF95FFA765AFD602B, 0x0101001088883000, 0x0140022128000401, 0x0D14204200800411, 0x4002020041004200, 0x43FF9A5CF4CA0C01, 0x4BFFCD8E7C587601,
			0xFC0FF2865334F576, 0xFC0BF6CE5924F576, 0x1000070401048082, 0x00C8000484040020, 0x0900001002020A2B, 0x00001102108D0020, 0xC3FFB7DC36CA8C89, 0xC3FF8A54F4CA2C89,
			0xFFFFFCFCFD79EDFF, 0xFC0863FCCB147576, 0x0000500104290400, 0x1104154202208824, 0x0308090010021A04, 0x00C0890448502100, 0xFC087E8E4BB2F736, 0x43FF9E4EF4CA2C89
		};

		const constexpr auto ROOK_OFFSETS = []() {
			const int shifts[] =
			{
				52, 53, 53, 53, 53, 53, 53, 52,
				53, 54, 54, 54, 54, 54, 54, 53,
				53, 54, 54, 54, 54, 54, 54, 53,
				53, 54, 54, 54, 54, 54, 54, 53,
				53, 54, 54, 54, 54, 54, 54, 53,
				53, 54, 54, 54, 54, 54, 54, 53,
				54, 55, 55, 55, 55, 55, 55, 54,
				53, 54, 54, 54, 54, 53, 54, 53
			};

			std::array<std::pair<int, int>, 64 + 1> offsets{};

			offsets[0].first = shifts[0];
			offsets[0].second = 0;

			for (int square = 1; square < 64; square++) {
				offsets[square].first = shifts[square];
				offsets[square].second = offsets[square - 1].second + (1 << (64 - shifts[square - 1]));
			}

			offsets[64].second = offsets[64 - 1].second + (1 << (64 - shifts[64 - 1]));

			return offsets;
		}();

		const constexpr auto BISHOP_OFFSETS = []() {
			const int shifts[] =
			{
				59, 60, 59, 59, 59, 59, 60, 59,
				60, 60, 59, 59, 59, 59, 60, 60,
				60, 60, 57, 57, 57, 57, 60, 60,
				59, 59, 57, 55, 55, 57, 59, 59,
				59, 59, 57, 55, 55, 57, 59, 59,
				60, 60, 57, 57, 57, 57, 60, 60,
				60, 60, 59, 59, 59, 59, 60, 60,
				59, 60, 59, 59, 59, 59, 60, 59
			};

			std::array<std::pair<int, int>, 64 + 1> offsets{};

			offsets[0].first = shifts[0];
			offsets[0].second = 0;

			for (int square = 1; square < 64; square++) {
				offsets[square].first = shifts[square];
				offsets[square].second = offsets[square - 1].second + (1 << (64 - shifts[square - 1]));
			}

			offsets[64].second = offsets[64 - 1].second + (1 << (64 - shifts[64 - 1]));

			return offsets;
		}();

		const constexpr int ROOK_MAXOFFSET = ROOK_OFFSETS[64].second;

		const constexpr int BISHOP_MAXOFFSET = BISHOP_OFFSETS[64].second;

		const constexpr auto ROOK_LOOKUP = []() {
			std::array<U64, ROOK_MAXOFFSET> lookup{};

			for (int square = 0; square < 64; square++) {
				U64 mask = ROOK_MASKS[square];
				int permutationCount = 1 << popCount(mask);

				for (int i = 0; i < permutationCount; i++) {
					U64 blockers = blockersPermutation(i, mask);
					U64 attacks = ZERO;
					int rank = getRank(square);
					int file = getFile(square);

					for (int rank_ = rank + 1; rank_ <= 7; rank_++) {
						attacks |= ONE << getSquare(file, rank_);
						if (blockers & (ONE << getSquare(file, rank_))) {
							break;
						}
					}

					for (int file_ = file + 1; file_ <= 7; file_++) {
						attacks |= ONE << getSquare(file_, rank);
						if (blockers & (ONE << getSquare(file_, rank))) {
							break;
						}
					}

					for (int rank_ = rank - 1; rank_ >= 0; rank_--) {
						attacks |= ONE << getSquare(file, rank_);
						if (blockers & (ONE << getSquare(file, rank_))) {
							break;
						}
					}

					for (int file_ = file - 1; file_ >= 0; file_--) {
						attacks |= ONE << getSquare(file_, rank);
						if (blockers & (ONE << getSquare(file_, rank))) {
							break;
						}
					}

					U64 key = transform(blockers, ROOK_MAGICS[square], ROOK_OFFSETS[square].first);
					int offset = ROOK_OFFSETS[square].second;

					lookup[offset + key] = attacks;
				}
			}

			return lookup;
		}();

		const constexpr auto BISHOP_LOOKUP = []() {
			std::array<U64, BISHOP_MAXOFFSET> lookup{};

			for (int square = 0; square < 64; square++) {
				U64 mask = BISHOP_MASKS[square];
				int permutationCount = 1 << popCount(mask);

				for (int i = 0; i < permutationCount; i++) {
					U64 blockers = blockersPermutation(i, mask);
					U64 attacks = ZERO;
					int file = getFile(square), file_ = 0;
					int rank = getRank(square), rank_ = 0;

					for (rank_ = rank + 1, file_ = file + 1; rank_ <= 7 && file_ <= 7; rank_++, file_++) {
						attacks |= ONE << getSquare(file_, rank_);
						if (blockers & (ONE << getSquare(file_, rank_))) {
							break;
						}
					}

					for (rank_ = rank - 1, file_ = file + 1; rank_ >= 0 && file_ <= 7; rank_--, file_++) {
						attacks |= ONE << getSquare(file_, rank_);
						if (blockers & (ONE << getSquare(file_, rank_))) {
							break;
						}
					}

					for (rank_ = rank - 1, file_ = file - 1; rank_ >= 0 && file_ >= 0; rank_--, file_--) {
						attacks |= ONE << getSquare(file_, rank_);
						if (blockers & (ONE << getSquare(file_, rank_))) {
							break;
						}
					}

					for (rank_ = rank + 1, file_ = file - 1; rank_ <= 7 && file_ >= 0; rank_++, file_--) {
						attacks |= ONE << getSquare(file_, rank_);
						if (blockers & (ONE << getSquare(file_, rank_))) {
							break;
						}
					}

					U64 key = transform(blockers, BISHOP_MAGICS[square], BISHOP_OFFSETS[square].first);
					int offset = BISHOP_OFFSETS[square].second;

					lookup[offset + key] = attacks;
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
	
	inline U64 pawn(int pieceSquare, Color color) {
		U64 pawn = ONE << pieceSquare;

		if (color == WHITE) {
			return ((pawn & ~U64_FILE_A) << 7) | ((pawn & ~U64_FILE_H) << 9);
		} else {
			return ((pawn & ~U64_FILE_A) >> 9) | ((pawn & ~U64_FILE_H) >> 7);
		}
	}

	inline U64 pawnKingside(U64 pieces, Color color) {
		return color == WHITE ? (pieces & ~U64_FILE_H) << 9 : (pieces & ~U64_FILE_H) >> 7;
	}

	inline U64 pawnQueenside(U64 pieces, Color color) {
		return color == WHITE ? (pieces & ~U64_FILE_A) << 7 : (pieces & ~U64_FILE_A) >> 9;
	}

	inline U64 knight(int pieceSquare) {
		return details::KNIGHT_LOOKUP[pieceSquare];
	}

	inline U64 bishop(int pieceSquare, U64 blockers) {
		U64 mask = BISHOP_MASKS[pieceSquare];
		U64 magic = details::BISHOP_MAGICS[pieceSquare];
		std::pair<int, int> offset = details::BISHOP_OFFSETS[pieceSquare];

		int key = details::transform(blockers & mask, magic, offset.first);
		return details::BISHOP_LOOKUP[offset.second + key];
	}

	inline U64 rook(int pieceSquare, U64 blockers) {
		U64 mask = ROOK_MASKS[pieceSquare];
		U64 magic = details::ROOK_MAGICS[pieceSquare];
		std::pair<int, int> offset = details::ROOK_OFFSETS[pieceSquare];

		int key = details::transform(blockers & mask, magic, offset.first);
		return details::ROOK_LOOKUP[offset.second + key];
	}

	inline U64 queen(int pieceSquare, U64 blockers) {
		return rook(pieceSquare, blockers) | bishop(pieceSquare, blockers);
	}

	inline U64 king(int pieceSquare) {
		return details::KING_LOOKUP[pieceSquare];
	}

	inline U64 slider(int pieceSquare, U64 blockers, Piece type) {
		switch (type) {
		case BISHOP: return bishop(pieceSquare, blockers);
		case ROOK: return rook(pieceSquare, blockers);
		case QUEEN: return queen(pieceSquare, blockers);
		default: return ZERO;
		}
	}
}

#endif