
def minimax(game_state):

	l_next_states = game_state 

	# check if terminal state
	if l_next_states == 0:
		# gamma:... replace with eval func?
		if isXWin():
			return 1
		elif isOWin():
			return -1
		else:
			return 0

	# QUESTION for Mark: 
	# Since we won't go through the whole tree, 
	# where do we put the evaluation function? In place of gamma?

	children = pState.findPossibleMoves(l_next_states);

	for child in children:

		if player == 'A':
			best_possible = float("-inf")
			# for each child in mu(A,state)
			for state in l_next_states:
				v = minimax(state,player)
				best_possible = max(best_possible,v)

				# max here would use evaluate on state in v

		else: # player == 'B'
			best_possible = float("inf")
			# for each child in mu(A,state)
			for state in l_next_states:
				v = minimax(state,player)
				best_possible = min(best_possible,v)

	return best_possible



def simple_evaluation(game_state,player):

	if player == 'A':
		player_symbol = 'x'
	else:
		player_symbol = 'o'

	for row in rows:
		total += row.count(player_symbol)
	for col in columns:
		total += col.count(player_symbol)
	for diag in diagonals:
		total = diag.count(player_symbol)

	return total


def evaluation_gamestate_for_alpha_beta_pruning(game_state, player):

	'''
	We need to evaluate states to determine which is the best 
	(for simple alpha-beta pruning). More complex by "pretending" to make 
	next moves, then reevaluating all of the moves again
	'''

	# game_state example
	# ....o..xox......


	''' set_of_winning_combinations
		0,1,2,3
		4,5,6,7
		8,9,10,11,
		12,13,14,15

		0,4,8,12; 	% 4 = 0
		1,5,9,13; 	% 4 = 1
		2,6,10,14; 	% 4 = 2
		3,7,11,15; 	% 4 = 3

		0,5,10,15 	% 5 = 0
		3,6,9,12	% 3 = 0
	'''

	# To determine points of player x, check lines for x's and o's 
	# Get points for conquering or obstructing winning positions
	for row in board:
		if 'x' in row and 'o' not in row:
			score_x += 1
		if 'o' in row and 'x' not in row:
			score_o += 1

	for column in board:
		if 'x' in column and 'o' not in column:
			score_x += 1
		if 'o' in column and 'x' not in column:
			score_o += 1
	
	for diagonal in board:
		if 'x' in diagonal and 'o' not in diagonal:
			score_x += 1
		if 'o' in diagonal and 'x' not in diagonal:
			score_o += 1

	# if player is A then score_x will be positive
	return score_x - score_o



def alternative_evaluation(game_state, player):

	'''
	checking the winning states:
	for each of the possible ways to make four-in-a-row, see if it contains a circle.
	if it does then its blocked.

	OR

	for each possible way to make four-in-a-row thats left, put an x. 
	if it also makes the amount of possibilities less for the o, its a good move.
	'''

	# which winning positions are still available?
	score_x = 10
	for row in board:
		if 'x' not in row:
			score_x -= 1
	for column in board:
		if 'x' not in column:
			score_x -= 1
	for diagonal in board:
		if 'x' not in diagonal:
			score_x -= 1





	