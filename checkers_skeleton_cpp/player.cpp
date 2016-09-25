#include "player.hpp"
#include <cstdlib>

#include <tr1/unordered_map>
#include <tr1/unordered_set>
//http://stackoverflow.com/questions/2179946/i-would-like-to-see-a-hash-map-example-in-c

namespace checkers
{

GameState Player::play(const GameState &pState,const Deadline &pDue)
{
    //std::cerr << "Processing " << pState.toMessage() << std::endl;

    std::vector<GameState> lNextStates;
    pState.findPossibleMoves(lNextStates);

    std::tr1::unordered_map<std::string,int> my_map;
	my_map["answer"] = 42;
	fprintf(stderr,"The answer to life and everything is: %d\n", my_map["answer"] );
	// hash-function requires C formatting for print
	// http://stackoverflow.com/questions/16819354/equivalent-of-formatted-printf-c-to-formatted-cerr-in-c


    if (lNextStates.size() == 0) return GameState(pState, Move());

    /*
     * Here you should write your clever algorithms to get the best next move, ie the best
     * next state. This skeleton returns a random move instead.
     */
    return lNextStates[rand() % lNextStates.size()];
}

/*namespace checkers*/ }
