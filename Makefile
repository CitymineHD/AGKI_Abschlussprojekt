all:
	gcc game.c network.c board.c -o agent_test -lm -g
release:
	gcc game.c network.c board.c -o agent_test -lm