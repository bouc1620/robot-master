#include <iostream>
#include <string>

#include "move-gen.h"
#include "board.h"
#include "move.h"

int main() {
	std::string input;
  
  Board board = Board();
  MoveGen moveGen = MoveGen(board);
  MoveList moves = moveGen.getLegalMoves();

	while (moves.size() > 0 && (input != "exit" && input != "quit")) {
    std::cout << "\n" << board.toString() << "\n";
    
    for (Move move : moves) {
      std::cout << move.toString() << " ";
    }
    
    Color toMove = board.getToMove();
    
    std::cout << "\n\n" << (toMove == WHITE ? "white" : "black");
    std::cout << " to play, enter a move... ";
    
		std::cin >> input;
    
    for (Move move : moves) {
      if (move.toString() == input) {
        board.doMove(move);
        break;
      }
    }
    
    moves = moveGen.getLegalMoves();
	}
  
  if (moveGen.isCheckmate()) {
    Color toMove = board.getToMove();
    
    std::cout << "\n" << board.toString();
    std::cout << "\ncheckmate, ";
    std::cout << (toMove == WHITE ? "black" : "white") << " wins!\n";
  } else if (moves.size() == 0) {
    std::cout << "\n" << board.toString() << "\nstalemate, the game is drawn\n";
  }
}