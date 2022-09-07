C = gcc
CXX = g++
RM = rm -f
PY = python3
CPPFLAGS = -O3 -std=c++17

SRCS = $(patsubst %,src/%,game.cpp piece.cpp bits.cpp masks.cpp)
OBJS = $(subst .cpp,.o,$(SRCS))

all: tool

tool: $(OBJS)
	$(CXX) $(CPPFLAGS) $(SRCS) src/perft.cpp -o perft.exe

cython:
	$(PY) GUI/setup.py build_ext

clean:
	$(RM) $(OBJS)
	$(RM) GUI/*.pyd GUI/*.so
	rm -r build