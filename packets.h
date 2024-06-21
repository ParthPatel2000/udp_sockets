#ifndef PACKETS_H
#define PACKETS_H

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <memory>
#include <cstring>
#include <iomanip>
#include <unordered_map>

/*TODO: ADD different packets for different types of messages
        Add the different Constructors for the different types of packets so that the user can create the packets easily
        also add functionalities for fragmentation and reassembly of packets as per the size of the message

        Message Types:  1 - ack
                        2 - chat
                        3 - join
                        4 - cards
                        5 - game_state_update

*/
// Example packet structures

class Packets
{

private:
    std::unordered_map<uint16_t, std::string> packet_buffer;
    uint16_t packet_id = 0;
    uint16_t MTU = 1500;
    uint16_t MAX_HEADER_SIZE = 225;
    uint16_t game_id = 1;

    void increment_packet_id();
    std::string create_packet(std::vector<std::string> message_JSON);
    std::vector<std::string> fragment_packet(const std::string &message);
    std::string get_header_JSON(
        uint16_t size,
        uint16_t type,
        uint16_t fragments = 0,
        uint16_t fragment_seq = 0,
        uint32_t timestamp = 0,
        uint32_t player_id = 0,
        uint32_t game_id = 0,
        uint32_t session_id = 0,
        uint32_t checksum = 0,
        uint8_t priority = 0,
        uint32_t sequence_number = 0);
    std::string message_JSON(const std::string &message);

public:
    Packets() = default;
    Packets(uint16_t mtu, uint16_t game_id) : MTU(mtu), game_id(game_id){};
    ~Packets() = default;
    std::string get_ack_JSON(uint16_t client_message_id);
    std::vector<std::string> get_packets(uint8_t packetType, const std::string &message);
    void remove_packet(uint16_t id);
    std::string fetch_packet(uint16_t id);
};

#endif // PACKETS_H
