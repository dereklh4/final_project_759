# Warnings
WFLAGS	:= #-Wall -Wextra -Wsign-conversion -Wsign-compare

# Optimization and architecture
OPT		:= -O3 -g
ARCH   	:= -march=native

# Language standard
CCSTD	:= -std=c99
CXXSTD	:= -std=c++11

# Linker options
LDOPT 	:= $(OPT)
LDFLAGS :=

EXEC	:= main.exe ReversiServer.class
OBJS:= main.o Player.class
CPPFILES := client_player.cpp client_baseline_minimax.cpp client_random_player.cpp node.cpp main.cpp

.DEFAULT_GOAL := all

.PHONY: debug
debug : OPT  := -O0 -g -fno-omit-frame-pointer -fsanitize=address
debug : LDFLAGS := -fsanitize=address
debug : ARCH :=
debug : $(EXEC)

all : Makefile clean $(EXEC)

ReversiServer.class: ReversiServer.java Player.java
	@ echo Compiling $@...
	@ javac ReversiServer.java
	
%.o : %.c $(CPPFILES) Makefile
	@ echo Compiling $<...
	$(CC) $(CCSTD) -g $(WFLAGS) $(OPT) $(ARCH) $(CFLAGS) -c $< -o $@

%.o : %.cpp $(CPPFILES) Makefile
	@ echo Compiling $<...
	$(CXX) $(CXXSTD) -g $(WFLAGS) $(OPT) $(ARCH) $(CXXFLAGS) -c $< -o $@

main.exe : main.o $(CPPFILES) Makefile
	@ echo Building $@...
	@ $(CXX) -g -o $@ $< $(LDFLAGS)

.PHONY: clean
clean:
	@ rm -f $(EXEC) $(OBJS)
