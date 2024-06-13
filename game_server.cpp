#include "game_server.h"

GameServer::GameServer(int port) 
    : port(port), 
      socket(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)),
      heartbeat_timer(io_service) 
{}

int GameServer::run()
{
    try
    {
        start_receive();
        start_heartbeat();
        
        io_service.run();  // Run the io_service to process asynchronous events
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
        [this](boost::system::error_code ec, std::size_t bytes_recvd) {
            if (!ec && bytes_recvd > 0)
            {
                std::string client_key = remote_endpoint.address().to_string() + ":" + std::to_string(remote_endpoint.port());
                if (clients.find(client_key) == clients.end())
                {
                    clients[client_key] = remote_endpoint;
                    std::string message = "Hi from server";
                    socket.send_to(boost::asio::buffer(message), remote_endpoint);
                }

                std::cout << "Received from " << remote_endpoint.address().to_string() << ": " << std::string(buffer, bytes_recvd) << std::endl;
            }
            start_receive();
        }
    );
}

void GameServer::start_heartbeat()
{
    heartbeat_timer.expires_after(std::chrono::seconds(5));
    heartbeat_timer.async_wait([this](const boost::system::error_code& ec) {
        if (!ec)
        {
            send_heartbeat();
            start_heartbeat();  // Reschedule the heartbeat
        }
    });
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