INC = -I./src -I./src/headers -fPIC
LDFLAGS = $(INC) 
CC = gcc
AR = ar

main: clean tests libmac

linux: clean tests liblinux

tests: test_bitboards test_bitutils parse_game

test_bitboards: clean
	$(CC) src/test/bitboards.c src/*.c $(LDFLAGS) -o ./build/test_bitboards

test_bitutils: clean
	$(CC) src/test/bitutils.c src/*.c $(LDFLAGS) -o ./build/test_bitutils

parse_game:
	$(CC) src/test/parse_game.c src/*.c $(LDFLAGS) -o ./build/parse_games

compile_lib: clean
	$(CC) $(LDFLAGS) -fno-common -c src/bitboard.c
	$(CC) $(LDFLAGS) -fno-common -c src/bitutils.c
	mv *.o build/lib

libmac: compile_lib
	$(CC) -dylib -flat_namespace -undefined suppress -dynamiclib -install_name '@executable_path/src/lib/libchess_smoengine.dylib' -current_version 1.0 -o ./build/lib/libchess_smoengine.dylib ./build/lib/bitboard.o ./build/lib/bitutils.o

liblinux: compile_lib
	$(CC) -shared -o ./build/lib/libchess_smoengine.so ./build/lib/bitboard.o ./build/lib/bitutils.o

clean:
	rm -rf build && mkdir -p ./build/lib 2>/dev/null
