''' checkers, gamestate:

here, there are
	GameState::reversed() - !! for symmetries!


Tips from last year student:
- check symmetries. make "templates"
- ending state lookup table (hash)
	save a portion of the states. smart ones!
- kattis uses EOG(). Always have move ready 
	for when its called
- 3! (factorial) means three moves left. don't 
	do heuristic here! you have time to make tree.





hash-function for checkers:
do modulus score_max