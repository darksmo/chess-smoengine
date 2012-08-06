INC = -I./src
LDFLAGS = $(INC) 
CC = gcc

main: clean
	$(CC) -g src/*.c $(LDFLAGS) -o engine

clean:
	rm -rf engine
