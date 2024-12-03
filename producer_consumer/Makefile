.PHONY: all clean

CC := gcc
VALG_LAGS := -Wall -ggdb3

all:
	$(CC) -o exam main.c -lpthread $(VALG_LAGS)
valgrind:
	valgrind --leak-check=yes ./exam
clean:
	rm -rf  exam 
