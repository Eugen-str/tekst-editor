DEBUG=-Wall -O0 -g3 -pedantic -std=c99
LIBS=-lncursesw

editor: src/*.c
	$(CC) -o editor $^ $(DEBUG) $(LIBS)
