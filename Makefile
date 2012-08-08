INC = -I./src
LDFLAGS = $(INC) 
CC = gcc

main: clean tests

tests: test_bitboards parse_game

test_bitboards:
	mkdir build 2>/dev/null && $(CC) src/test/bitboards.c src/*.h src/*.c $(LDFLAGS) -o ./build/test_bitboards

parse_game:
	$(CC) src/test/parse_game.c src/*.h src/*.c $(LDFLAGS) -o ./build/parse_games

clean:
	rm -rf build
