# Warnings
WFLAGS	:= #-Wall -Wextra -Wsign-conversion -Wsign-compare

# Optimization and architecture
OPT		:= -O3
ARCH   	:= -march=native

# Language standard
CCSTD	:= -std=c99
CXXSTD	:= -std=c++11

# Linker options
LDOPT 	:= $(OPT)
LDFLAGS :=

EXEC	:= client_random_player.exe ReversiServer.class
OBJS:= client_random_player.o Player.class

.DEFAULT_GOAL := all

.PHONY: debug
debug : OPT  := -O0 -g -fno-omit-frame-pointer -fsanitize=address
debug : LDFLAGS := -fsanitize=address
debug : ARCH :=
debug : $(EXEC)

all : Makefile $(EXEC)

ReversiServer.class: ReversiServer.java Player.java
	@ echo Compiling $@...
	@ javac ReversiServer.java
	
client_random_player.exe : client_random_player.o
	@ echo Building $@...
	@ $(CXX) -o $@ $<
	
client_random_player.o: client_random_player.cpp Makefile
	@ echo Compiling $<...
	@ $(CXX) $(CXXSTD) $(WFLAGS) $(OPT) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@ rm -f $(EXEC) $(OBJS)
