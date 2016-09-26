#include "minimax.h"

#include <algorithm>
#include <climits>


namespace TICTACTOE3D
{
	static const double TIME_BUFFER = 0.1;
	static const double PRELIM_TIME_BUFFER = 1.5;

	static const int PRELIM_SORT_DEPTH = 1;
	static const int MINMAX_ALPHA_BETA_MAX_DEPTH = 3;

	TICTACTOE3D::GameState MiniMax::get_best_next_state(const Deadline &pDue, const GameState& current_state, uint8_t our_player_type)
	{
		GameStateEvaluation eval = minimax_alpha_beta(pDue, current_state, our_player_type, MINMAX_ALPHA_BETA_MAX_DEPTH, 0, -INT_MAX, INT_MAX);

		return eval.state;
	}

	MiniMax::GameStateEvaluation MiniMax::minimax_alpha_beta(const Deadline &pDue, const GameState& current_state, uint8_t our_player_type, const int max_depth, int depth, int alpha, int beta)
	{
		std::vector<GameState> l_next_states;

		current_state.findPossibleMoves(l_next_states);
		int num_next_moves = l_next_states.size();

		if (depth == 0 && num_next_moves > 1)
		{
			prelim_sort(pDue, our_player_type, l_next_states);
		}

		if (num_next_moves == 0 || depth >= max_depth || pDue - Deadline::now() < TIME_BUFFER)
		{
			return{ current_state, evaluate_gamestate_3d(current_state, our_player_type) };
		}
		else
		{
			if (current_state.getNextPlayer() != our_player_type) //A
			{
				GameStateEvaluation best_next_state{ current_state, -INT_MAX };

				int iter = 0;
				for (GameState next_state : l_next_states)
				{
					//cerr << "A:" << "iter:" << iter << ":of:" << num_next_moves << endl;
					GameStateEvaluation next_state_eval = minimax_alpha_beta(pDue, next_state, our_player_type, max_depth, depth + 1, alpha, beta);
					if (next_state_eval.value > best_next_state.value)
					{
						best_next_state = GameStateEvaluation{ next_state, next_state_eval.value };
					}
					if (best_next_state.value > alpha)
					{
						alpha = best_next_state.value;
					}
					if (alpha >= beta || pDue - Deadline::now() < TIME_BUFFER)
					{
						break;
					}
					++iter;
				}
				return best_next_state;
			}
			else //B
			{
				GameStateEvaluation best_next_state{ current_state, INT_MAX };
				for (GameState next_state : l_next_states)
				{
					int iter = 0;
					//cerr << "A:" << "iter:" << iter << ":of:" << num_next_moves << endl;
					GameStateEvaluation next_state_eval = minimax_alpha_beta(pDue, next_state, our_player_type, max_depth, depth + 1, alpha, beta);
					if (next_state_eval.value < best_next_state.value)
					{
						best_next_state = GameStateEvaluation{ next_state, next_state_eval.value };
					}
					if (next_state_eval.value < beta)
					{
						beta = next_state_eval.value;
					}
					if (beta <= alpha || pDue - Deadline::now() < TIME_BUFFER)
					{
						break;
					}
					++iter;
				}
				return best_next_state;
			}
		}
	}

	int MiniMax::evaluate_gamestate_3d(const GameState& game_state, const int our_player_type)
	{
		int win_lose_scalar = 500;
		if (game_state.isEOG()) {
			if (game_state.isXWin())
			{
				if (our_player_type == CELL_X)
					return win_lose_scalar;
				else
					return -win_lose_scalar;
			}
			else if (game_state.isOWin())
			{
				if (our_player_type == CELL_O)
					return win_lose_scalar;
				else
					return -win_lose_scalar;
			}
			else if (game_state.isDraw())
			{
				return win_lose_scalar - 400;
			}
		}

		vector<int> p_values =
		{
			//////////////////////////////////////////////////////////////////////////
			//4_pts
			//////////////////////////////////////////////////////////////////////////
			0, 3, 12, 15,
			21, 22, 25, 26,
			37, 38, 41, 42,
			48, 51, 60, 63,
			//////////////////////////////////////////////////////////////////////////
			//2_pts
			//////////////////////////////////////////////////////////////////////////
			17, 18, 20, 23,
			24, 27, 29, 30,
			33, 34, 36, 39,
			40, 43, 45, 46
		};

		int index_p_4_max = 15;

		int x_value = 0;
		int o_value = 0;

		for (int i = 0; i < game_state.cSquares; ++i) //all rows
		{
			const uint8_t& cell_value = game_state.at(i);
			if (cell_value == CELL_X || cell_value == CELL_O)
			{
				int& value = cell_value == CELL_X ? x_value : o_value;

				auto it = find(p_values.begin(), p_values.end(), i);
				if (it == p_values.end()) {
					value += 1;
				}
				else {
					auto index = distance(p_values.begin(), it);
					value += index <= index_p_4_max ? 4 : 2;
				}
			}
		}

		if (our_player_type == CELL_X) {
			return x_value - o_value;
		}
		else {
			return o_value - x_value;
		}
	}

	void MiniMax::prelim_sort(const Deadline &pDue, uint8_t our_player_type, vector<GameState>& moves)
	{
		cerr << "prelim_start:" << pDue - Deadline::now() << endl;

		vector<GameStateEvaluation> evals;
		for (int i = 0; i < moves.size(); ++i)
		{
			evals.push_back({ moves[i], minimax(pDue, moves[i], our_player_type, PRELIM_SORT_DEPTH, 0).value });
		}

		sort(evals.begin(), evals.end());

		moves.clear();
		for (GameStateEvaluation eval : evals) {
			moves.push_back(eval.state);
		}

		cerr << "prelim_end:" << pDue - Deadline::now() << endl;
	}

	MiniMax::GameStateEvaluation MiniMax::minimax(const Deadline &pDue, const GameState& current_state, uint8_t our_player_type, const int max_depth, int depth)
	{
		std::vector<GameState> possible_next_states;
		current_state.findPossibleMoves(possible_next_states);
		int num_next_moves = possible_next_states.size();

		if (num_next_moves == 0 || depth >= max_depth || pDue - Deadline::now() < PRELIM_TIME_BUFFER)
		{
			return{ current_state, evaluate_gamestate_3d(current_state, our_player_type) };
		}
		else
		{
			if (current_state.getNextPlayer() != our_player_type) //A
			{
				GameStateEvaluation best_next_state{ current_state, -INT_MAX };
				for (GameState next_state : possible_next_states)
				{
					GameStateEvaluation next_state_eval = minimax(pDue, next_state, our_player_type, max_depth, depth + 1);
					if (next_state_eval.value > best_next_state.value)
					{
						best_next_state = GameStateEvaluation{ next_state, next_state_eval.value };
					}
					if (pDue - Deadline::now() < PRELIM_TIME_BUFFER)
						break;
				}
				return best_next_state;
			}
			else //B
			{
				GameStateEvaluation best_next_state{ current_state, INT_MAX };
				for (GameState next_state : possible_next_states)
				{
					GameStateEvaluation next_state_eval = minimax(pDue, next_state, our_player_type, max_depth, depth + 1);
					if (next_state_eval.value < best_next_state.value)
					{
						best_next_state = GameStateEvaluation{ next_state, next_state_eval.value };
					}
					if (pDue - Deadline::now() < PRELIM_TIME_BUFFER)
						break;
				}
				return best_next_state;
			}
		}
	}
}