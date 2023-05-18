CC = g++
CFLAGS = -O3

SRC = gre_fou.cpp
EXE = gre_fou
LIB = pcap


.PHONY: all clean

all:
	$(CC) $(CFLAGS) $(SRC) -o $(EXE) -l $(LIB)

clean:
	@rm -f $(EXE)