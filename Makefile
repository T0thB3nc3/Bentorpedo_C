CC=gcc
CFLAGS=-Wall -g -fopenmp
NCURSES_LIBS=`pkg-config --libs ncurses` 
# (üresre, ha a pkg_configot nem találja a windows)
NCURSES_CFLAGS=`pkg-config --cflags ncurses`

SRC=src/main.c src/game.c
OBJ=$(SRC:.c=.o)
TARGET=Bentorpedo

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(NCURSES_LIBS)
	rm -f $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) $(NCURSES_CFLAGS) -c $< -o $@

test: test_torpedo.o src/game.o
	$(CC) $(CFLAGS) -o test_torpedo test_torpedo.o src/game.o $(NCURSES_LIBS)

test_torpedo.o: test_torpedo.c src/game.h
	$(CC) $(CFLAGS) $(NCURSES_CFLAGS) -c test_torpedo.c -o test_torpedo.o

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean