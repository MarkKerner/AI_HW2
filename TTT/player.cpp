#include "player.hpp"
#include <cstdlib>

namespace TICTACTOE
{

GameState Player::play(const GameState &pState,const Deadline &pDue)
{
    std::vector<GameState> lNextStates;
    pState.findPossibleMoves(lNextStates);

    if (lNextStates.size() == 0) return GameState(pState, Move());

    /*
     * Here you should write your clever algorithms to get the best next move, ie the best
     * next state. This skeleton returns a random move instead.
     */
	int i;

    return lNextStates[rand() % lNextStates.size()];
}

/*namespace TICTACTOE*/ }
