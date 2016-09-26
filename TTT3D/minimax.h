#ifndef MINIMAX_H
#define MINIMAX_H
#include "gamestate.hpp"
#include "deadline.hpp"

using namespace std;
namespace TICTACTOE3D
{
	class MiniMax
	{
		struct GameStateEvaluation {
			GameState state;
			int value;

			bool operator < (const GameStateEvaluation& gse) const
			{
				return (value > gse.value);
			}
		} typedef GameStateEvaluation;

	public:
		static GameState get_best_next_state(const Deadline &pDue, const GameState& current_state, uint8_t our_player_type);
	private:
		static MiniMax::GameStateEvaluation minimax(const Deadline &pDue, const GameState& current_state, uint8_t our_player_type, const int max_depth, int depth);
		static GameStateEvaluation minimax_alpha_beta(const Deadline &pDue, const GameState& current_state, uint8_t our_player_type, const int max_depth, int depth, int alpha, int beta);
		static void prelim_sort(const Deadline &pDue, uint8_t our_player_type, vector<GameState>& moves);
		static int evaluate_gamestate_3d(const GameState& game_state, const int our_player_type);
	};
}
#endif // MINIMAX_H