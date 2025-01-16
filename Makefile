all:
	gcc game.c network.c board.c -o agent_test -lm -g -O0
release:
	gcc game.c network.c board.c -o agent_test -lm