# Compiler and flags
CXX := g++
CFLAGS := -Wall -Wextra -std=c++11

# Targets
all: server client game_server

game_server: game_server.cpp
	$(CXX) $(CFLAGS) -o game_server game_server.cpp

server: server.cpp
	$(CXX) $(CFLAGS) -o server server.cpp

client: client.cpp
	$(CXX) $(CFLAGS) -o client client.cpp

clean:
	rm -f server client game_server

remake: clean all
