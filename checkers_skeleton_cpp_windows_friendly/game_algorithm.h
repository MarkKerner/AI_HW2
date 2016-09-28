#pragma once

#include "gamestate.hpp"
#include "deadline.hpp"
#include <unordered_map>

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

	enum FLAG
	{
		EXACT,
		LOWERBOUND,
		UPPERBOUND
	};

	struct GameStateHashValue {
		float value;
		FLAG flag;
		int depth;
	} typedef GameStateHashValue;

	static vector<vector<int>> lookup_table;
	static unordered_map<int, GameStateHashValue> transposition_table;

public:
	static GameState get_best_move(const Deadline& p_due, const GameState& p_starting_state);
private:

	static GameStateEvaluation nega_max(const Deadline& p_due, const GameState& p_state, uint8_t our_player_type, int depth, int color, float alpha, float beta);
	static float evaluate_state(const GameState& p_state, const uint8_t our_player_type);
	static vector<vector<int>> init_zobris(int n_positions, int n_pieces);
	static int get_zobris_hash(const vector<vector<int>>& lookup_table, const GameState& state);
};