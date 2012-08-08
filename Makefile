INC = -I./src
LDFLAGS = $(INC) 
CC = gcc

main: clean
	mkdir build 2>/dev/null && $(CC) src/test/bitboards.c src/*.h src/*.c $(LDFLAGS) -o ./build/test_bitboards

clean:
	rm -rf build
