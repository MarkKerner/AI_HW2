#include "player.hpp"
#include <cstdlib>
#include <iostream>

using namespace std;

namespace TICTACTOE
{

GameState Player::play(const GameState &pState,const Deadline &pDue)
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


    return l_next_states[rand() % l_next_states.size()];
}

/*namespace TICTACTOE*/ }
