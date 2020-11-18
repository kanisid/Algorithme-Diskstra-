CC=g++
CFLAGS=-c -Wall -Wextra -pedantic -pedantic-errors -O3 -std=c++11

all: mon_programme

mon_programme: main.o Csv_parser.o
	$(CC) main.o Csv_parser.o -o mon_programme

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

csv_parser.o: Csv_parser.cpp Csv_parser.hpp
	$(CC) $(CFLAGS) Csv_parser.cpp

clean:
	rm -rf *o test
