INC = -I./src -I./src/headers
LDFLAGS = $(INC) 
CC = gcc
AR = ar

main: clean tests lib

tests: test_bitboards parse_game

test_bitboards: clean
	$(CC) src/test/bitboards.c src/*.c $(LDFLAGS) -o ./build/test_bitboards

parse_game:
	$(CC) src/test/parse_game.c src/*.c $(LDFLAGS) -o ./build/parse_games

compile_lib: clean
	$(CC) $(LDFLAGS) -fno-common -c src/bitboard.c
	$(CC) $(LDFLAGS) -fno-common -c src/bitutils.c
	mv *.o build/lib

lib: compile_lib
	$(CC) -dylib -flat_namespace -undefined suppress -dynamiclib -install_name '@executable_path/src/lib/libchess_smoengine.dylib' -current_version 1.0 -o ./build/lib/libchess_smoengine.dylib ./build/lib/bitboard.o ./build/lib/bitutils.o

clean:
	rm -rf build && mkdir -p ./build/lib 2>/dev/null
