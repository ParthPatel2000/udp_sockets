#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <thread>

std::string name = "Client";

void send_heartbeat(boost::asio::ip::udp::socket &socket, const boost::asio::ip::udp::endpoint &server_endpoint, boost::asio::steady_timer &timer);

int main(int argc, char *argv[])
{
    std::string server_ip = argc >= 3 ? argv[1] : "0.0.0.0";
    int server_port = argc >= 3 ? std::stoi(argv[2]) : 50000;
    name = argc == 4 ? argv[3] : "Client";

    try
    {
        std::cout << "Connecting to server " << server_ip << " on port " << server_port << std::endl;

        boost::asio::io_service io_service;
        boost::asio::ip::udp::socket socket(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0));

        // Set up server endpoint
        boost::asio::ip::udp::endpoint server_endpoint(boost::asio::ip::address::from_string(server_ip), server_port);

        // Send initial message to server to register
        std::string initial_message = "Hello server";
        socket.send_to(boost::asio::buffer(initial_message), server_endpoint);

        // Buffer to store incoming data
        char buffer[1024];

        // Set up heartbeat timer
        boost::asio::steady_timer timer(io_service);
        send_heartbeat(socket, server_endpoint, timer);

        std::thread io_thread([&io_service]()
                              { io_service.run(); });

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

        io_thread.join();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

void send_heartbeat(boost::asio::ip::udp::socket &socket, const boost::asio::ip::udp::endpoint &server_endpoint, boost::asio::steady_timer &timer)
{
    timer.expires_after(std::chrono::seconds(5));
    timer.async_wait([&socket, &server_endpoint, &timer](const boost::system::error_code &ec)
                     {
        if (!ec)
        {
            std::string heartbeat_message = "Heartbeat from " + name;
            socket.send_to(boost::asio::buffer(heartbeat_message), server_endpoint);

            // Schedule the next heartbeat
            send_heartbeat(socket, server_endpoint, timer);
        } });
}
