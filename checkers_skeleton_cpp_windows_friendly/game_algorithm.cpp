
#include "game_algorithm.h"
#include <float.h>
#include <cstdlib>
#include <chrono>

static const int MAX_DEPTH = 10;
static const double LOWER_TIME_LIMIT = 0.001;

static const double time_left(const Deadline& p_due)
{
	return p_due - Deadline::now();
}

vector<vector<int>> GameAlgorithm::lookup_table = GameAlgorithm::init_zobris(32, 2);

unordered_map<int, GameAlgorithm::GameStateHashValue> GameAlgorithm::transposition_table;

checkers::GameState GameAlgorithm::get_best_move(const Deadline& p_due, const GameState& p_starting_move)
{
	transposition_table = unordered_map<int, GameStateHashValue>();
	return nega_max(p_due, p_starting_move, p_starting_move.getNextPlayer(), MAX_DEPTH, 1, -FLT_MAX, FLT_MAX).state;
}

GameAlgorithm::GameStateEvaluation GameAlgorithm::nega_max(const Deadline& p_due, const GameState& p_state, uint8_t our_player_type, int depth, int color, float alpha, float beta)
{
	float alpha_orig = alpha;
	//////////////////////////////////////////////////////////////////////////
	if (p_state.getNextPlayer() == our_player_type)
	{
		auto hashed_state_it = transposition_table.find(get_zobris_hash(lookup_table, p_state));
		if (hashed_state_it != transposition_table.end())
		{
			GameStateHashValue hashed_state = hashed_state_it->second;
			if (hashed_state.depth >= depth)
			{
				switch (hashed_state.flag)
				{
				case EXACT:
					return{ p_state, hashed_state.value };
				case LOWERBOUND:
					if (hashed_state.value > alpha)
						alpha = hashed_state.value;
					break;
				case UPPERBOUND:
					if (hashed_state.value < beta)
						beta = hashed_state.value;
					break;
				}
				if (alpha >= beta)
					return{ p_state, hashed_state.value };
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	vector<GameState> next_states;
	p_state.findPossibleMoves(next_states);
	int num_next_states = next_states.size();

	if (depth == 0 || num_next_states == 0)
	{
		float state_evaluation = evaluate_state(p_state, our_player_type);
		return GameStateEvaluation{ p_state, color * state_evaluation };
	}
	//////////////////////////////////////////////////////////////////////////
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

		if (time_left(p_due) < LOWER_TIME_LIMIT)
			return best_value;
		if (alpha >= beta) {
			break;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	if (p_state.getNextPlayer() == our_player_type) {
		GameStateHashValue new_hashtable_value;
		new_hashtable_value.value = best_value.value;
		if (best_value.value <= alpha_orig)
			new_hashtable_value.flag = UPPERBOUND;
		else if (best_value.value >= beta)
			new_hashtable_value.flag = LOWERBOUND;
		else
			new_hashtable_value.flag = EXACT;
		new_hashtable_value.depth = depth;

		transposition_table[get_zobris_hash(lookup_table, p_state)] = new_hashtable_value;
	}
	//////////////////////////////////////////////////////////////////////////
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
		{
			red_count += cell_status&CELL_KING ? 2 : 1;
		}
		else if (cell_status&CELL_WHITE)
		{
			white_count += cell_status&CELL_KING ? 2 : 1;
		}
	}

	if (our_player_type == CELL_RED)
		return red_count / (((float)white_count));
	else
		return white_count / (((float)red_count));
}

std::vector<std::vector<int>> GameAlgorithm::init_zobris(int n_positions, int n_pieces)
{
	//init rand
	chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
	srand(ms.count());

	vector<vector<int>> table(n_positions, vector<int>(n_pieces));
	for (int i = 0; i < n_positions; ++i)
	{
		for (int j = 0; j < n_pieces; ++j)
		{
			table[i][j] = rand();
		}
	}
	return table;
}

/************************************************************************/
/* function hash(board):
	   h := 0
	   for i from 1 to 64:      # loop over the board positions
		   if board[i] != empty:
			   j := the piece at board[i], as listed in the constant indices, above
			   h := h XOR table[i][j]
	   return h                                                                     */
	   /************************************************************************/

int GameAlgorithm::get_zobris_hash(const vector<vector<int>>& lookup_table, const GameState& board_state)
{
	int hash = 0;
	for (int i = 0; i < board_state.cSquares; ++i)
	{
		if (board_state.at(i) != CELL_EMPTY)
		{
			int j;
			if (board_state.at(i)&CELL_RED)
				j = 0;
			else
				j = 1;

			hash ^= lookup_table[i][j];
		}
	}
	return hash;
}

