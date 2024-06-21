#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include "packets.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <boost/asio.hpp>
#include <thread>
#include <memory>
#include <simdjson.h>

using timestamp = std::chrono::time_point<std::chrono::steady_clock>;

class GameServer
{
private:
    Packets packets_util = Packets(1500, 69);
    int port = 50000;
    int mtu = 1024;              // Maximum Transmission Unit
    int disconnect_timeout = 10; // Timeout in seconds  to disconnect inactive clients
    simdjson::dom::parser parser;
    simdjson::dom::element doc;
    boost::asio::io_service io_service;
    boost::asio::ip::udp::socket socket;
    boost::asio::steady_timer heartbeat_timer;  // Heartbeat timer
    boost::asio::steady_timer disconnect_timer; // Timer to disconnect inactive clients
    std::unordered_map<std::string, boost::asio::ip::udp::endpoint> clients;
    std::unordered_map<std::string, timestamp> client_last_response;
    char buffer[1024];
    boost::asio::ip::udp::endpoint remote_endpoint;

public:
    GameServer(int port);
    int run();
    void send_heartbeat();

private:
    void start_receive();   // Start asynchronous receive
    void start_heartbeat(); // Start asynchronous heartbeat
    void async_send(const std::string &message, const boost::asio::ip::udp::endpoint &endpoint);
    void disconnect_inactive_clients();
    void send_ack(uint16_t client_message_id, const boost::asio::ip::udp::endpoint &endpoint);
};

#endif // GAME_SERVER_H
