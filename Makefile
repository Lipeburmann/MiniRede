CXX ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -pedantic -O2

.PHONY: all run clean

all: minirede

minirede: minirede.cpp minirede.h
	$(CXX) $(CXXFLAGS) minirede.cpp -o minirede

run: minirede
	python3 gui/server.py

clean:
	rm -f minirede
