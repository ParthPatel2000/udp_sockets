#include <iostream>
#include <string>
#include <unordered_map>
#include <boost/asio.hpp>

int main()
{
    int port = 50000;

    try
    {
        boost::asio::io_service io_service;
        boost::asio::ip::udp::socket socket(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port));

        // Map to store client endpoints
        std::unordered_map<std::string, boost::asio::ip::udp::endpoint> clients;

        // Buffer to store incoming data
        char buffer[1024];

        while (true)
        {
            // Endpoint to store sender's information
            boost::asio::ip::udp::endpoint remote_endpoint;
            boost::system::error_code error;

            // Receive data from any client
            size_t len = socket.receive_from(boost::asio::buffer(buffer), remote_endpoint, 0, error);

            if (error && error != boost::asio::error::message_size)
            {
                throw boost::system::system_error(error);
            }

            // Register client if not already registered
            std::string client_key = remote_endpoint.address().to_string() + ":" + std::to_string(remote_endpoint.port());
            if (clients.find(client_key) == clients.end())
            {
                clients[client_key] = remote_endpoint;
            }

            // Print received message
            std::cout << "Received from " << remote_endpoint.address().to_string() << ": " << std::string(buffer, len) << std::endl;

            // Send response to all clients
            std::string message = "Hi from server";
            for (const auto& client : clients)
            {
                socket.send_to(boost::asio::buffer(message), client.second);
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
