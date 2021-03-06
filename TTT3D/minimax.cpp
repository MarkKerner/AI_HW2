#include "minimax.h"

#include <algorithm>
#include <climits>
#include <cmath>


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
			return{ current_state, evaluate_gamestate_3d_2(current_state, our_player_type) };
		}
		else
		{
			if (current_state.getNextPlayer() != our_player_type) //A
			{
				GameStateEvaluation best_next_state{ current_state, -INT_MAX };

				int iter = 0;
				for (GameState next_state : l_next_states)
				{
					//cerr << "A:" << "iter:" << iter << ":of:" << num_next_moves << ":alpha:" << alpha << ":beta:" << beta << endl;
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
						//cerr << "pruned:" << "alpha:" << alpha << ":beta:" << beta << endl;
						return best_next_state;
					}
					++iter;
				}
				return best_next_state;
			}
			else //B
			{
				GameStateEvaluation best_next_state{ current_state, INT_MAX };
				int iter = 0;
				for (GameState next_state : l_next_states)
				{
					//cerr << "B:" << "iter:" << iter << ":of:" << num_next_moves << ":alpha:" << alpha << ":beta:" << beta << endl;
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
						//cerr << "pruned:" << "alpha:" << alpha << ":beta:" << beta << endl;
						return best_next_state;
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
			return{ current_state, evaluate_gamestate_3d_2(current_state, our_player_type) };
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

	static void evaluate_row_points(const int& contains_x, const int& contains_o, vector<int>& x_row_values, vector<int>& o_row_values)
	{
		int x_value = 0;
		int o_value = 0;

		if (!contains_x && !contains_o)
		{
			x_value = 1;
			o_value = 1;
		}
		else if (!contains_o)
			x_value += pow(2, contains_x);
		else if (!contains_x)
			o_value += pow(2, contains_o);

		x_row_values.push_back(x_value);
		o_row_values.push_back(o_value);
	}

	int MiniMax::evaluate_gamestate_3d_2(const GameState& game_state, const int our_player_type)
	{

		//cerr << game_state.toString(our_player_type) << endl;
		int win_lose_scalar = INT_MAX;
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
				return win_lose_scalar - 1000;
			}
		}

		vector<int> buckets;

		vector<int> x_row_values;
		vector<int> o_row_values;

		int contains_x;
		int contains_o;
		//////////////////////////////////////////////////////////////////////////
		//rows
		//////////////////////////////////////////////////////////////////////////
		for (int row = 0; row < game_state.cSquares; row += 4) //all rows
		{
			contains_x = 0;
			contains_o = 0;

			for (int cell = row; cell < row + 4; ++cell)
			{
				buckets.push_back(cell);

				const uint8_t& cell_value = game_state.at(cell);
				if (cell_value == CELL_X)
					++contains_x;
				else if (cell_value == CELL_O)
					++contains_o;
			}
			evaluate_row_points(contains_x, contains_o, x_row_values, o_row_values);
		}

		/*
		for (int i=0;i<buckets.size();++i){
			cerr << " " << buckets[i];
			if(i % 4 == 0 && i!=0) cerr << endl;
		}
		exit(0);*/

		//////////////////////////////////////////////////////////////////////////
		//cols
		//////////////////////////////////////////////////////////////////////////
		for (int level = 0; level < game_state.cSquares; level += 16)
		{
			for (int col = level; col < level + 4; ++col) //all columns
			{
				contains_x = 0;
				contains_o = 0;
				for (int cell = col; cell < level + 16; cell += 4)
				{

					buckets.push_back(cell);

					const uint8_t& cell_value = game_state.at(cell);
					if (cell_value == CELL_X)
						++contains_x;
					else if (cell_value == CELL_O)
						++contains_o;
				}
				evaluate_row_points(contains_x, contains_o, x_row_values, o_row_values);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		//row diagonals top to bottom
		//////////////////////////////////////////////////////////////////////////
		for (int row = 0; row < 16; row += 4)
		{
			contains_x = 0;
			contains_o = 0;
			for (int level = row; level < game_state.cSquares; level += 16 + 1)
			{
				buckets.push_back(level);

				const uint8_t& cell_value = game_state.at(level);
				if (cell_value == CELL_X)
					++contains_x;
				else if (cell_value == CELL_O)
					++contains_o;
			}
			evaluate_row_points(contains_x, contains_o, x_row_values, o_row_values);
		}

		for (int row = 3; row < 16; row += 4)
		{
			contains_x = 0;
			contains_o = 0;
			for (int level = row; level < game_state.cSquares - 3; level += 16 - 1)
			{
				buckets.push_back(level);
				const uint8_t& cell_value = game_state.at(level);
				if (cell_value == CELL_X)
					++contains_x;
				else if (cell_value == CELL_O)
					++contains_o;
			}
			evaluate_row_points(contains_x, contains_o, x_row_values, o_row_values);
		}
		//////////////////////////////////////////////////////////////////////////
		//col diagonals top to bottom
		//////////////////////////////////////////////////////////////////////////
		for (int col = 0; col < 4; ++col) //all columns
		{
			contains_x = 0;
			contains_o = 0;
			for (int level = col; level < game_state.cSquares; level += 20)
			{
				buckets.push_back(level);
				const uint8_t& cell_value = game_state.at(level);
				if (cell_value == CELL_X)
					++contains_x;
				else if (cell_value == CELL_O)
					++contains_o;

			}
			evaluate_row_points(contains_x, contains_o, x_row_values, o_row_values);
		}
		for (int col = 12; col < 16; ++col) //all columns
		{
			contains_x = 0;
			contains_o = 0;
			for (int level = col; level < game_state.cSquares - 4; level += 12)
			{
				buckets.push_back(level);
				const uint8_t& cell_value = game_state.at(level);
				if (cell_value == CELL_X)
					++contains_x;
				else if (cell_value == CELL_O)
					++contains_o;

			}
			evaluate_row_points(contains_x, contains_o, x_row_values, o_row_values);
		}

		//////////////////////////////////////////////////////////////////////////
		//top_left-bottom_right diagonal
		//////////////////////////////////////////////////////////////////////////
		for (int layer = 0; layer < game_state.cSquares; layer += 16) {
			contains_x = 0;
			contains_o = 0;
			for (int cell = 0 + layer; cell < layer + 16; cell += 5)
			{
				buckets.push_back(cell);
				const uint8_t& cell_value = game_state.at(cell);
				if (cell_value == CELL_X)
					++contains_x;
				else if (cell_value == CELL_O)
					++contains_o;
			}
			evaluate_row_points(contains_x, contains_o, x_row_values, o_row_values);
		}
		//////////////////////////////////////////////////////////////////////////
		//top_right-bottom_left diagonal
		//////////////////////////////////////////////////////////////////////////
		for (int layer = 0; layer < game_state.cSquares; layer += 16) {
			contains_x = 0;
			contains_o = 0;
			for (int cell = layer + 3; cell < layer + 16 - 3; cell += 3)
			{
				buckets.push_back(cell);
				const uint8_t& cell_value = game_state.at(cell);
				if (cell_value == CELL_X)
					++contains_x;
				else if (cell_value == CELL_O)
					++contains_o;
			}
			evaluate_row_points(contains_x, contains_o, x_row_values, o_row_values);
		}
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		//diagonal diagonal
		//////////////////////////////////////////////////////////////////////////
		contains_x = 0;
		contains_o = 0;
		for (int i = 0; i < game_state.cSquares; i += 21)
		{
			buckets.push_back(i);
			const uint8_t& cell_value = game_state.at(i);
			if (cell_value == CELL_X)
				++contains_x;
			else if (cell_value == CELL_O)
				++contains_o;
		}
		evaluate_row_points(contains_x, contains_o, x_row_values, o_row_values);
		//////////////////////////////////////////////////////////////////////////
		contains_x = 0;
		contains_o = 0;
		for (int i = 3; i < game_state.cSquares; i += 19)
		{
			buckets.push_back(i);
			const uint8_t& cell_value = game_state.at(i);
			if (cell_value == CELL_X)
				++contains_x;
			else if (cell_value == CELL_O)
				++contains_o;
		}
		evaluate_row_points(contains_x, contains_o, x_row_values, o_row_values);
		//////////////////////////////////////////////////////////////////////////
		contains_x = 0;
		contains_o = 0;
		for (int i = 12; i < game_state.cSquares; i += 13)
		{
			buckets.push_back(i);
			const uint8_t& cell_value = game_state.at(i);
			if (cell_value == CELL_X)
				++contains_x;
			else if (cell_value == CELL_O)
				++contains_o;
		}
		evaluate_row_points(contains_x, contains_o, x_row_values, o_row_values);
		//////////////////////////////////////////////////////////////////////////
		contains_x = 0;
		contains_o = 0;
		for (int i = 15; i < 49; i += 11)
		{
			buckets.push_back(i);
			const uint8_t& cell_value = game_state.at(i);
			if (cell_value == CELL_X)
				++contains_x;
			else if (cell_value == CELL_O)
				++contains_o;
		}
		evaluate_row_points(contains_x, contains_o, x_row_values, o_row_values);

		////////////////////////////////////////////////////////////////////////// 
		//top-down 
		////////////////////////////////////////////////////////////////////////// 
		for (int i = 0; i < 16; ++i)
		{
			contains_x = 0;
			contains_o = 0;
			for (int j = i; j < game_state.cSquares; j += 16)
			{

				buckets.push_back(j);
				const uint8_t& cell_value = game_state.at(j);
				if (cell_value == CELL_X)
					++contains_x;
				else if (cell_value == CELL_O)
					++contains_o;
			}
			evaluate_row_points(contains_x, contains_o, x_row_values, o_row_values);
		}

		int x_value = 0;
		int o_value = 0;
		int n_x_eight = 0;
		int n_o_eight = 0;
		int n_x_four = 0;
		int n_o_four = 0;
		for (int i = 0; i < x_row_values.size(); ++i)
		{
			int& x_value_in_row = x_row_values[i];
			int& o_value_in_row = o_row_values[i];
			if(x_value_in_row == 8)
				++n_x_eight;
			if(o_value_in_row == 8)
				++n_o_eight;
			if(x_value_in_row == 4)
				++n_x_four;
			if(o_value_in_row == 4)
				++n_o_four;
			x_value += x_value_in_row;
			o_value += o_value_in_row;
		}
		if (n_x_eight > 1 && n_o_eight == 0)
			x_value *= 2;
		if (n_o_eight > 1 && n_x_eight == 0)
			o_value *= 2;

		if (our_player_type == CELL_X)
		{
			return x_value - o_value;
		} 
		else
		{
			return o_value - x_value;
		}
		/*cerr << x_row_values.size() << ":" << o_row_values.size() << endl;
		for (int i = 0; i < x_row_values.size(); ++i)
			cerr << x_row_values[i] << ":";
		cerr << endl;
		/*for (int i = 0; i < o_row_values.size(); ++i)
			cerr << o_row_values[i] << ":";
		cerr << endl;*/
		//////////////////////////////////////////////////////////////////////////
		//testing
		/*int how_many_rows = 0;
		for (int i = 0; i < buckets.size(); ++i) {
			if (i > 0 && i % 4 == 0) {
				cerr << endl;
				cerr << "\t\t +=" << buckets[i - 1] - buckets[i - 2];
				cerr << endl;
				++how_many_rows;
			}
			cerr << " " << buckets[i];
		}
		cerr << endl << "there are: " << how_many_rows << " rows..." << endl;
		exit(0);*/
		//////////////////////////////////////////////////////////////////////////
	}
}