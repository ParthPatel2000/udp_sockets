# Compiler and flags
CXX := g++
CFLAGS := -Wall -Wextra -std=c++11

lib := -lboost_system -lboost_thread -lpthread -lsimdjson

# Targets
all: client game_server

game_server: game_server.cpp
	$(CXX) $(CFLAGS) game_server.cpp packets.cpp -o game_server $(lib)


client: client.cpp
	$(CXX) $(CFLAGS)  client.cpp packets.cpp -o client $(lib)

clean:
	rm -f server client game_server

remake: clean all
