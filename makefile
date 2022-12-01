C=gcc
CXX=g++
RM=rm -f
PY=python3
CPPFLAGS=-O3 -std=c++20

SRCS = $(patsubst %,src/%,game.cpp piece.cpp bits.cpp moves.cpp masks.cpp perft.cpp uci.cpp magic.cpp)
OBJS = $(subst .cpp,.o,$(SRCS))

all: tool

tool: $(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) src/main.cpp -o main.exe

cython:
	$(PY) GUI/setup.py build_ext

magic: src/bits.o src/piece.o src/masks.o src/magic.o
	$(CXX) $(CPPFLAGS) src/bits.o src/piece.o src/masks.o src/magic.o src/generate_magics.cpp -o generate_magics.exe

clean:
	$(RM) $(OBJS)
	$(RM) GUI/*.pyd GUI/*.so
	rm -rf build
