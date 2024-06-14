#include "game_server.h"

// TODO: While writing the client make sure to also implement timeout from the client side for non responsive server situation

GameServer::GameServer(int port)
    : port(port),
      socket(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)),
      heartbeat_timer(io_service),
      disconnect_timer(io_service)
{
}

int GameServer::run()
{
    try
    {
        start_receive();
        start_heartbeat();
        disconnect_inactive_clients();
        io_service.run(); // Run the io_service to process asynchronous events
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

void GameServer::start_receive()
{
    socket.async_receive_from(
        boost::asio::buffer(buffer), remote_endpoint,
        [this](boost::system::error_code ec, std::size_t bytes_recvd)
        {
            handle_receive(ec, bytes_recvd);
            start_receive(); // Reschedule the receive
        });
}

void GameServer::handle_receive(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    if (!error && bytes_transferred > 0)
    {
        // client_key is the IP address and port of the client
        std::string client_key = remote_endpoint.address().to_string() + ":" + std::to_string(remote_endpoint.port());

        // If the client is not in the list of clients, add it and send the new client ack message
        if (clients.find(client_key) == clients.end())
        {
            // Add the client to the list of clients
            clients[client_key] = remote_endpoint;
            // Record the time of the last response from the client
            client_last_response[client_key] = std::chrono::steady_clock::now();
            std::string message = "Connection established. Welcome!";
            // Send the new client an ack message
            socket.send_to(boost::asio::buffer(message), remote_endpoint);
        }
        // Print the received message and the sender's IP address
        std::cout << "Received from " << remote_endpoint.address().to_string() << ": " << std::string(buffer, bytes_transferred) << std::endl;
        // Record the time of the last response from the client
        client_last_response[client_key] = std::chrono::steady_clock::now();
    }
}

void GameServer::async_send(const std::string &message, const boost::asio::ip::udp::endpoint &endpoint)
{
    socket.async_send_to(boost::asio::buffer(message), endpoint,
                         [](const boost::system::error_code &ec, std::size_t bytes_sent)
                         {
                             if (ec)
                             {
                                 std::cerr << "Error sending message: " << ec.message() << std::endl;
                             }
                         });
}

/**
 * @brief Disconnects clients that have been inactive for more than 10 seconds asynchronously
 *
 */
void GameServer::disconnect_inactive_clients()
{
    disconnect_timer.expires_after(std::chrono::seconds(disconnect_timeout));
    disconnect_timer.async_wait([this](const boost::system::error_code &ec)
                                {
        if (!ec)
        {
            // Get the current time
            timestamp now = std::chrono::steady_clock::now();
            // Iterate through all clients
            for (auto it = client_last_response.begin(); it != client_last_response.end();)
            {
                // If the client has been inactive for more than disconnect timeout seconds, disconnect the client
                if (now - it->second > std::chrono::seconds(disconnect_timeout))
                {
                    std::cout << "Client " << it->first << " has been inactive for "<<this->disconnect_timeout <<" seconds. Disconnecting..." << std::endl;
                    async_send("You have been disconnected due to inactivity", clients[it->first]);
                    clients.erase(it->first);
                    it = client_last_response.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            disconnect_inactive_clients(); // Reschedule the disconnect timer
        } });
}

void GameServer::start_heartbeat()
{
    heartbeat_timer.expires_after(std::chrono::seconds(5));
    heartbeat_timer.async_wait([this](const boost::system::error_code &ec)
                               {
        if (!ec)
        {
            send_heartbeat();
            start_heartbeat();  // Reschedule the heartbeat
        } });
}

void GameServer::send_heartbeat()
{
    std::string message = "Heartbeat";
    for (const auto &client : clients)
    {
        socket.send_to(boost::asio::buffer(message), client.second);
    }
}

int main()
{
    GameServer server(50000);
    return server.run();
}