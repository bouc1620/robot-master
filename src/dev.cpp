#include <iostream>
#include <string>
#include <limits>
#include <chrono>
#include <random>

#include "bitboard.h"
#include "move-gen.h"
#include "board.h"
#include "move.h"
#include "dev.h"

static std::mt19937 gen((std::random_device())());
static std::uniform_int_distribution<U64> U64dist(0, std::numeric_limits<U64>::max());
static std::uniform_int_distribution<int> intdist(0, std::numeric_limits<int>::max());

unsigned long long perft(const Board* board, int depth) {
	if (depth == 0) {
		return 1;
	}

	MoveGen* moveGen = new MoveGen(*board);

	unsigned long long nodes = 0;

	for (Move move : moveGen->getLegalMoves()) {
		Board* newBoard = new Board(*board);

		newBoard->doMove(move);
		nodes += perft(newBoard, depth - 1);

		delete newBoard;
	}

	delete moveGen;

	return nodes;
}

void perftDivide(int depth, std::string FEN) {
	unsigned long long total = 0;

	Board board(FEN);
	MoveGen moveGen(board);

	std::cout << "\n";

	auto startTime = std::chrono::steady_clock::now();

	for (Move move : moveGen.getLegalMoves()) {
		Board* newBoard = new Board(board);

		newBoard->doMove(move);

		unsigned long long res = perft(newBoard, depth - 1);
		total += res;

		std::cout << move.toString() << ": " << res << "\n";

		delete newBoard;
	}
	
	auto endTime = std::chrono::steady_clock::now();
	std::chrono::duration<double> duration = endTime - startTime;

	std::cout << "\n-----------------------------------\n";
	std::cout << "Total time (ms) : " << static_cast<int>(duration.count() * 1000) << "\n";
	std::cout << "Nodes searched	: " << total << "\n";
	std::cout << "Nodes / second	: " << static_cast<int>(total / duration.count()) << "\n";
}

void printU64(U64 board, std::string name) {
	if (name != "") {
		std::cout << "\n ---- " << name << " ----";
	}

	std::cout << "\n";

	for (int square = 56; square >= 0; square++) {
		if (board & (ONE << square)) {
			std::cout << "x ";
		} else {
			std::cout << ". ";
		}

		if ((square + 1) % 8 == 0) {
			std::cout << '\n';
			square += 2 * -8;
		}
	}

	std::cout << "\n";
}

int randomSquare() {
	return intdist(gen) % 64;
}

U64 randomU64() {
	return U64dist(gen);
}