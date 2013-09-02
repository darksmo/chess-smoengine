INC = -I./src -I./src/headers -fPIC
LDFLAGS = $(INC)
CC = gcc
AR = ar

main: clean tests libmac

linux: clean tests liblinux

tests: test_bitboards test_bitutils test_engine parse_game

test_bitboards: clean
	$(CC) -g src/test/bitboards.c src/*.c $(LDFLAGS) -o ./build/test_bitboards

test_bitutils: clean
	$(CC) -g src/test/bitutils.c src/*.c $(LDFLAGS) -o ./build/test_bitutils

test_engine: clean
	$(CC) -g src/test/engine.c src/*.c $(LDFLAGS) -o ./build/test_engine

parse_game:
	$(CC) src/test/parse_game.c src/*.c $(LDFLAGS) -o ./build/parse_games

compile_lib: clean
	$(CC) $(LDFLAGS) -O3 -fno-common -c src/bitboard.c
	$(CC) $(LDFLAGS) -O3 -fno-common -c src/bitutils.c
	$(CC) $(LDFLAGS) -O3 -fno-common -c src/engine.c
	mv *.o build/lib

libmac: compile_lib
	$(CC) -O3 -dylib -flat_namespace -undefined suppress -dynamiclib -install_name '@executable_path/src/lib/libchess_smoengine.dylib' -current_version 1.0 -o ./build/lib/libchess_smoengine.dylib ./build/lib/bitboard.o ./build/lib/bitutils.o ./build/lib/engine.o

liblinux: compile_lib
	$(CC) -O3 -shared -o ./build/lib/libchess_smoengine.so ./build/lib/bitboard.o ./build/lib/bitutils.o ./build/lib/engine.o

clean:
	rm -rf build && mkdir -p ./build/lib 2>/dev/null
