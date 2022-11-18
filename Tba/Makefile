all: clean makedir project

CC=gcc

CFLAGS = -O2 -MD -Wall
LIBS=-lm

# distination folder
DIST=bin

project: project.c
	$(CC) $(CFLAGS) -o bin/project project.c board.c bot.c utils.c $(LIBS)


makedir:
	@mkdir $(DIST)

clean:
	@rm -fr $(DIST)
