#include <iostream>
#include <string>
#include <boost/asio.hpp>

int main()
{
    int server_port = 3333;

    try
    {
        boost::asio::io_service io_service;
        boost::asio::ip::udp::socket socket(io_service);
        socket.open(boost::asio::ip::udp::v4());

        // Set up server endpoint
        boost::asio::ip::udp::endpoint server_endpoint(boost::asio::ip::address::from_string("127.0.0.1"), server_port);

        // Send initial message to server to register
        std::string initial_message = "Hello server";
        socket.send_to(boost::asio::buffer(initial_message), server_endpoint);

        // Buffer to store incoming data
        char buffer[1024];

        while (true)
        {
            // Endpoint to store sender's information
            boost::asio::ip::udp::endpoint remote_endpoint;
            boost::system::error_code error;

            // Receive data from the server (or any sender)
            size_t len = socket.receive_from(boost::asio::buffer(buffer), remote_endpoint, 0, error);

            if (error && error != boost::asio::error::message_size)
            {
                throw boost::system::system_error(error);
            }

            // Print the received message
            std::cout << "Received from " << remote_endpoint.address().to_string() << ": " << std::string(buffer, len) << std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
