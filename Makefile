# Compiler and flags
CXX := g++
CFLAGS := -Wall -Wextra -std=c++11

# Targets
all: server client

server: server.cpp
	$(CXX) $(CFLAGS) -o server server.cpp

client: client.cpp
	$(CXX) $(CFLAGS) -o client client.cpp

clean:
	rm -f server client

remake: clean all
