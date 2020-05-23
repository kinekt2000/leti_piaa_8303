CC = g++
CFLAGS = -g -std=c++17


all:
	@echo "choose program [aho_corasick_search, wildcard_search]"

aho_corasick_search: main_aho_corasick.o aho_corasick_search.o
	$(CC) $(CFLAGS) main_aho_corasick.o aho_corasick_search.o -o aho_corasick_search

main_aho_corasick.o: main_aho_corasick.cpp
	$(CC) $(CFLAGS) -c main_aho_corasick.cpp

aho_corasick_search.o: aho_corasick_search.cpp aho_corasick_search.h trie_node.h
	$(CC) $(CFLAGS) -c aho_corasick_search.cpp


wildcard_search: main_wildcard.o wildcard_search.o
	$(CC) $(CFLAGS) main_wildcard.o wildcard_search.o -o wildcard_search

main_wildcard.o: main_wildcard.cpp
	$(CC) $(CFLAGS) -c main_wildcard.cpp

wildcard_search.o: wildcard_search.cpp wildcard_search.h trie_node.h
	$(CC) $(CFLAGS) -c wildcard_search.cpp

clean:
	rm -rf *.o aho_corasick_search wildcard_search
