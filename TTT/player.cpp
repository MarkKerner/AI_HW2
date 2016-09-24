#include "player.hpp"
#include <cstdlib>
#include <iostream>
#include <stack>

#include <climits>

#include "minimax.h"

using namespace std;

namespace TICTACTOE
{

	GameState Player::play(const GameState &pState, const Deadline &pDue)
	{
		std::vector<GameState> l_next_states;
		pState.findPossibleMoves(l_next_states);

		// findPossibleMoves is mu. lNextStates is an array of possible next states to choose
		if (l_next_states.size() == 0)
			return GameState(pState, Move());

		/*
		 * Here you should write your clever algorithms to get the best next move, ie the best
		 * next state. This skeleton returns a random move instead.
		 */

		GameState best_move = MiniMax::minimax(pDue, pState, pState.getNextPlayer() ^ (CELL_X | CELL_O), 4);

		assert(pState.getNextPlayer() != best_move.getNextPlayer());

		return best_move;
	}

	/*namespace TICTACTOE*/
}
