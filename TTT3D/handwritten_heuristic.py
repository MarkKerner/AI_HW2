'''
# new heuristics function

# we know how to reach rows and columns

# then verticals are % 16 = 0

From the number they start, add +k for each step

# diagonal - across, TR_BL
# 3, 
# 19, 
# 35, 
# 51, 
	+3

# diagonal - across, TL_BR
# 0, 
# 16,
# 32,
# 48,
	+5

diagonal - inside
0,21,42,63
3,22,41,60
12,25,38,51
15,26,37,48

vertical, TL_
0,
16,
32,
48,

3,
19,
35
51
	+20

vertical, TR_
12,
28,
44,
60,

15
31
47
63
	+12



