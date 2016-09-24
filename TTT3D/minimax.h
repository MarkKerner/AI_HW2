#ifndef MINIMAX_H
#define MINIMAX_H
#include "gamestate.hpp"
#include "deadline.hpp"
namespace TICTACTOE3D
{

	class MiniMax
	{
		struct GameStateEvaluation {
			GameState state;
			int value;
		} typedef GameStateEvaluation;
		static const double TIME_BUFFER;
	public:
		static GameState minimax(const Deadline &pDue, const GameState& current_state, uint8_t our_player_type, const int max_depth);
	private:
		static GameStateEvaluation minimax(const Deadline &pDue, const GameState& current_state, uint8_t our_player_type, const int max_depth, int depth, int alpha, int beta);
		static int evaluate_gamestate_3d(const GameState& game_state, const int our_player_type);
	};
}
#endif // MINIMAX_H