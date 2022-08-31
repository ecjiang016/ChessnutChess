C=gcc
CXX=g++
RM = rm -f
CPPFLAGS=-O3 -std=c++17

SRCS=$(patsubst %,src/%,game.cpp piece.cpp bits.cpp)
OBJS=$(subst .cpp,.o,$(SRCS))

all:tool

tool: $(OBJS)
	$(CXX) $(CPPFLAGS) $(SRCS) src/perft.cpp -o perft.exe

clean:
	$(RM) $(OBJS)
	$(RM) GUI/*.pyd
	rm -r build