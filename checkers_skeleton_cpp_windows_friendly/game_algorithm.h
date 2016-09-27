#pragma once

#include "gamestate.hpp"
#include "deadline.hpp"

using namespace checkers;
using namespace std;

class GameAlgorithm
{
	struct GameStateEvaluation {
		GameState state;
		float value;

		bool operator < (const GameStateEvaluation& gse) const
		{
			return (value > gse.value);
		}
	} typedef GameStateEvaluation;

public:
	static GameState get_best_move(const Deadline& p_due, const GameState& p_starting_state);
private:
	static GameStateEvaluation nega_max(const Deadline& p_due, const GameState& p_state, uint8_t our_player_type, int depth, int color);
	static float evaluate_state(const GameState& p_state, const uint8_t our_player_type);
};