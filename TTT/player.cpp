#include "player.hpp"
#include <cstdlib>
#include <iostream>
#include <stack>

#include <climits>

using namespace std;

namespace TICTACTOE
{

	struct GameStateEvaluation {
		GameState state;
		int value;
	} typedef GameStateEvaluation;

	static int evaluate_gamestate(const GameState& game_state, const int our_player_type)
	{
		if (game_state.isXWin())
		{
			if (our_player_type == CELL_X)
				return 1;
			else return -1;
		}
		else if (game_state.isOWin())
		{
			if (our_player_type == CELL_O)
				return 1;
			else return -1;
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

	static const double TIME_BUFFER = 0.3;

	//how to always return the best value?
	static GameStateEvaluation minimax(const Deadline &pDue, const GameState& current_state, uint8_t our_player_type, const int max_depth, int depth, int alpha, int beta)
	{
		vector<GameState> possible_next_states;
		current_state.findPossibleMoves(possible_next_states);
		int num_next_moves = possible_next_states.size();

		if (num_next_moves == 0 || depth == max_depth || pDue.getSeconds() < TIME_BUFFER)
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

					if (beta <= alpha && pDue.getSeconds() < TIME_BUFFER)
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
					if (beta <= alpha && pDue.getSeconds() < TIME_BUFFER)
						break;
				}
				return best_next_state;
			}
		}
	}

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
		cerr << pDue.getSeconds() << endl;
		GameState best_move = minimax(pDue, pState, pState.getNextPlayer() ^ (CELL_X | CELL_O), 4, 0, -INT_MAX, INT_MAX).state;

		assert(pState.getNextPlayer() != best_move.getNextPlayer());

		return best_move;
	}

	/*namespace TICTACTOE*/
}
