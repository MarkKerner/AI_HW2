#include "player.hpp"
#include <cstdlib>

#include "minimax.h"

using namespace std;

namespace TICTACTOE3D
{

	GameState Player::play(const GameState &pState, const Deadline &pDue)
	{
		cerr << "play:" << (pState.getNextPlayer() != CELL_X ? "PLAYER_X" : "PLAYER_O") << endl;
		std::vector<GameState> lNextStates;
		pState.findPossibleMoves(lNextStates);

		if (lNextStates.size() == 0) return GameState(pState, Move());

		/*
		 * Here you should write your clever algorithms to get the best next move, ie the best
		 * next state. This skeleton returns a random move instead.
		 */
		GameState best_move = MiniMax::get_best_next_state(pDue, pState, pState.getNextPlayer() ^ (CELL_X | CELL_O));

		//assert(pState.getNextPlayer() != best_move.getNextPlayer());
		//assert(pState.getMove().getType() != best_move.getMove().getType());

		return best_move;
	}

	/*namespace TICTACTOE3D*/
}
