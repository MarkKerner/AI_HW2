#include "minimax.h"

namespace TICTACTOE
{
	const double MiniMax::TIME_BUFFER = 0.1;

	GameState MiniMax::minimax(const Deadline &pDue, const GameState& current_state, uint8_t our_player_type, const int max_depth)
	{
		GameStateEvaluation eval = minimax(pDue, current_state, current_state.getNextPlayer() ^ (CELL_X | CELL_O), max_depth, 0, -INT_MAX, INT_MAX);

		return eval.state;
	}

	GameStateEvaluation MiniMax::minimax(const Deadline &pDue, const GameState& current_state, uint8_t our_player_type, const int max_depth, int depth, int alpha, int beta)
	{
		std::vector<GameState> possible_next_states;
		current_state.findPossibleMoves(possible_next_states);
		int num_next_moves = possible_next_states.size();

		if (num_next_moves == 0 || depth == max_depth || pDue - Deadline::now() < TIME_BUFFER)
		{
			return{ current_state, evaluate_gamestate(current_state, our_player_type) };
		}
		else
		{
			if (current_state.getNextPlayer() != our_player_type) //A
			{
				GameStateEvaluation best_next_state{ current_state, -INT_MAX };
				for (GameState next_state : possible_next_states)
				{
					GameStateEvaluation next_state_eval = minimax(pDue, next_state, our_player_type, max_depth, depth + 1, alpha, beta);
					if (next_state_eval.value > best_next_state.value)
					{
						best_next_state = GameStateEvaluation{ next_state, next_state_eval.value };
					}
					if (best_next_state.value > alpha)
					{
						alpha = best_next_state.value;
					}

					if (beta <= alpha &&  pDue - Deadline::now() < TIME_BUFFER)
						break;
				}
				return best_next_state;
			}
			else //B
			{
				GameStateEvaluation best_next_state{ current_state, INT_MAX };
				for (GameState next_state : possible_next_states)
				{
					GameStateEvaluation next_state_eval = minimax(pDue, next_state, our_player_type, max_depth, depth + 1, alpha, beta);
					if (next_state_eval.value < best_next_state.value)
					{
						best_next_state = GameStateEvaluation{ next_state, next_state_eval.value };
					}
					if (next_state_eval.value < beta)
					{
						beta = next_state_eval.value;
					}
					if (beta <= alpha &&  pDue - Deadline::now() < TIME_BUFFER)
						break;
				}
				return best_next_state;
			}
		}
	}

	int MiniMax::evaluate_gamestate(const GameState& game_state, const int our_player_type)
	{
		int win_lose_scalar = 5;
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
			else return -win_lose_scalar;
		}

		int x_value = 0;
		int o_value = 0;

		bool contains_x;
		bool contains_o;
		//////////////////////////////////////////////////////////////////////////
		//rows
		//////////////////////////////////////////////////////////////////////////
		for (int row = 0; row < game_state.cSquares; row += 4) //all rows
		{
			contains_x = false;
			contains_o = false;
			for (int cell = row; cell < row + 4; ++cell)
			{
				const uint8_t& cell_value = game_state.at(cell);
				if (cell_value == CELL_X)
					contains_x = true;
				else if (cell_value == CELL_O)
					contains_o = true;
			}
			if (contains_x && !contains_o)
				++x_value;
			else if (contains_o && !contains_x)
				++o_value;
		}
		//////////////////////////////////////////////////////////////////////////
		//cols
		//////////////////////////////////////////////////////////////////////////
		for (int col = 0; col < 4; ++col) //all columns
		{
			contains_x = false;
			contains_o = false;
			for (int cell = col; cell < game_state.cSquares; cell += 4)
			{
				const uint8_t& cell_value = game_state.at(cell);
				if (cell_value == CELL_X)
					contains_x = true;
				else if (cell_value == CELL_O)
					contains_o = true;
			}
			if (contains_x && !contains_o)
				++x_value;
			else if (contains_o && !contains_x)
				++o_value;
		}
		//////////////////////////////////////////////////////////////////////////
		//top_left-bottom_right diagonal
		//////////////////////////////////////////////////////////////////////////
		contains_x = false;
		contains_o = false;
		for (int cell = 0; cell < game_state.cSquares; cell += 5)
		{
			const uint8_t& cell_value = game_state.at(cell);
			if (cell_value == CELL_X)
				contains_x = true;
			else if (cell_value == CELL_O)
				contains_o = true;
		}
		if (contains_x && !contains_o)
			++x_value;
		else if (contains_o && !contains_x)
			++o_value;
		//////////////////////////////////////////////////////////////////////////
		//top_right-bottom_left diagonal
		//////////////////////////////////////////////////////////////////////////
		contains_x = false;
		contains_o = false;
		for (int cell = 3; cell < game_state.cSquares; cell += 3)
		{
			const uint8_t& cell_value = game_state.at(cell);
			if (cell_value == CELL_X)
				contains_x = true;
			else if (cell_value == CELL_O)
				contains_o = true;
		}
		if (contains_x && !contains_o)
			++x_value;
		else if (contains_o && !contains_x)
			++o_value;
		if (our_player_type == CELL_X)
		{
			return x_value - o_value;
		}
		else {
			return o_value - x_value;
		}
	}
}