
#include "game_algorithm.h"
#include <float.h>

static const double LOWER_TIME_LIMIT = 0.1;

static const double time_left(const Deadline& p_due)
{
	return p_due - Deadline::now();
}

checkers::GameState GameAlgorithm::get_best_move(const Deadline& p_due, const GameState& p_starting_move)
{
	return nega_max(p_due, p_starting_move, p_starting_move.getNextPlayer(), 5, 1, -FLT_MAX, FLT_MAX).state;
}

GameAlgorithm::GameStateEvaluation GameAlgorithm::nega_max(const Deadline& p_due, const GameState& p_state, uint8_t our_player_type, int depth, int color, float alpha, float beta)
{
	vector<GameState> next_states;
	p_state.findPossibleMoves(next_states);
	int num_next_states = next_states.size();

	if (depth == 0 || num_next_states == 0)
	{
		//cerr << (depth == 0) << ":" << (num_next_states == 0) << endl;
		float state_evaluation = evaluate_state(p_state, our_player_type);
		return GameStateEvaluation{ p_state, color * state_evaluation };
	}

	GameStateEvaluation best_value{ p_state, -FLT_MAX };

	for (const GameState& next_state : next_states)
	{
		GameStateEvaluation v = nega_max(p_due, next_state, our_player_type, depth - 1, -color, -beta, -alpha);
		v.value = -v.value; //Multiply by -1 when back-propagating

		if (v.value > best_value.value)
		{
			best_value.state = next_state;
			best_value.value = v.value;
		}
		if (v.value > alpha)
		{
			alpha = v.value;
		}

		if (alpha >= beta || time_left(p_due) < LOWER_TIME_LIMIT) {
			break;
		}
	}
	return best_value;
}

float GameAlgorithm::evaluate_state(const GameState& p_state, const uint8_t our_player_type)
{
	float win_scalar = FLT_MAX / 2.0f;

	if (p_state.isEOG())
	{
		if (p_state.isRedWin())
		{
			if (our_player_type == CELL_RED)
				return win_scalar;
			else
				return -win_scalar;
		}
		else if (p_state.isWhiteWin())
		{
			if (our_player_type == CELL_WHITE)
				return win_scalar;
			else
				return -win_scalar;
		}
		else if (p_state.isDraw())
		{
			return FLT_MAX / 4.0f; 
		}
	}

	int red_count = 0;
	int white_count = 0;

	for (int i = 0; i < p_state.cSquares; ++i)
	{
		const uint8_t& cell_status = p_state.at(i);
		if (cell_status&CELL_RED)
			++red_count;
		else if (cell_status&CELL_WHITE)
			++white_count;
	}

	if (our_player_type == CELL_RED)
		return red_count / (((float)white_count));
	else
		return white_count / (((float)red_count));
}

