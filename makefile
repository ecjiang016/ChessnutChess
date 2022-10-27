C=gcc
CXX=g++
RM=rm -f
PY=python3
CPPFLAGS=-O3 -std=c++17

SRCS = $(patsubst %,src/%,game.cpp piece.cpp bits.cpp masks.cpp perft.cpp uci.cpp)
OBJS = $(subst .cpp,.o,$(SRCS))

all: tool

tool: $(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) src/main.cpp -o main.exe

cython:
	$(PY) GUI/setup.py build_ext

clean:
	$(RM) $(OBJS)
	$(RM) GUI/*.pyd GUI/*.so
	rm -rf build
