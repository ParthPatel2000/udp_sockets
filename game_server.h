#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <boost/asio.hpp>
#include <thread>

class GameServer
{
private:
    int port;
    boost::asio::io_service io_service;
    boost::asio::ip::udp::socket socket;
    boost::asio::steady_timer heartbeat_timer;  // Heartbeat timer
    std::unordered_map<std::string, boost::asio::ip::udp::endpoint> clients;
    char buffer[1024];
    boost::asio::ip::udp::endpoint remote_endpoint;

public:
    GameServer(int port);
    int run();
    void send_heartbeat();

private:
    void start_receive();     // Start asynchronous receive
    void start_heartbeat();   // Start asynchronous heartbeat
    void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred);
};

#endif // GAME_SERVER_H
