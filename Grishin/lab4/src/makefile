CC = g++
CFLAGS = -g -std=c++17

all:
	@echo "choose programm [KMP_search, cyclic_shift, split_text]"

KMP_search: KMP_search.o
	$(CC) $(CFLAGS) KMP_search.o -o KMP_search

KMP_search.o: KMP_search.cpp
	$(CC) $(CFLAGS) -c KMP_search.cpp


cyclic_shift: cyclic_shift.o
	$(CC) $(CFLAGS) cyclic_shift.o -o cyclic_shift

cyclic_shift.o: cyclic_shift.cpp
	$(CC) $(CFLAGS) -c cyclic_shift.cpp

split_text: split_text.o
	$(CC) $(CFLAGS) split_text.o -o split_text

split_text.o: split_text.cpp
	$(CC) $(CFLAGS) -c split_text.cpp


clean:
	rm -rf *.o KMP_search cyclic_shift
