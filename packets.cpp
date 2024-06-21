#include <simdjson.h>
#include "packets.h"


std::vector<std::string> Packets::get_packets(uint8_t packetType, const std::string &message)
{
    std::vector<std::string> packets;
    std::vector<std::string> fragments = fragment_packet(message);
    uint8_t num_fragments = 0;
    std::string buffer;
    for (const auto &fragment : fragments)
    {
        switch (packetType)
        {
        case 2:
            buffer = "{" + get_header_JSON(fragment.size(), packetType, fragments.size(), num_fragments++);
            buffer += "," + message_JSON(fragment) + "}";
            packets.push_back(buffer);
            packet_buffer[packet_id] = buffer;
            break;
        default:
            break; // Add more cases for different packet types
        }
    }
    return packets;
}

/**
 * @brief Takes a vector of JSON strings and returns a single string that is the packet
 *        with header and message seperated by a newline
 *
 * @param message_JSON
 * @return std::string
 */
std::string Packets::create_packet(std::vector<std::string> message_JSON)
{
    std::string packet;
    packet += ("{" + message_JSON[0] + "," + message_JSON[1] + "}");
    return packet;
}

// Ack in JSON format
std::string Packets::get_ack_JSON(uint16_t client_message_id)
{
    std::stringstream ss;
    ss << R"({ack:)" << client_message_id
       << "}";
    return ss.str();
}

void Packets::increment_packet_id()
{
    if (packet_id == UINT16_MAX)
        packet_id = 0;
    else
        packet_id++;
}

// Remove packet from buffer after getting ack
void Packets::remove_packet(uint16_t id)
{
    packet_buffer.erase(id);
}

// Fetch packet from buffer if ack is not received to resend
std::string Packets::fetch_packet(uint16_t id)
{
    return packet_buffer[id];
}

// Header in JSON format
std::string Packets::get_header_JSON(
    uint16_t size,
    uint16_t type,
    uint16_t fragments,
    uint16_t fragment_seq,
    uint32_t timestamp,
    uint32_t player_id,
    uint32_t game_id,
    uint32_t session_id,
    uint32_t checksum,
    uint8_t priority,
    uint32_t sequence_number)
{
    std::ostringstream ss;
    ss << R"("header":{)"
       << R"("size":)" << size << R"(,)"
       << R"("type":)" << type << R"(,)"
       << R"("fragments":)" << fragments << R"(,)"
       << R"("fragment_seq":)" << fragment_seq << R"(,)"
       << R"("id":)" << packet_id << R"(,)"
       << R"("timestamp":)" << timestamp << R"(,)"
       << R"("player_id":)" << player_id << R"(,)"
       << R"("game_id":)" << this->game_id << R"(,)"
       << R"("session_id":)" << session_id << R"(,)"
       << R"("checksum":)" << checksum << R"(,)"
       << R"("priority":)" << static_cast<int>(priority) << R"(,)"
       << R"("sequence_number":)" << sequence_number
       << R"(})";
    increment_packet_id();
    return ss.str();
}

// Message in JSON format
std::string Packets::message_JSON(const std::string &message)
{
    std::stringstream ss;
    ss << R"("message":")" << message
       << R"(")";
    return ss.str();
}

std::vector<std::string> Packets::fragment_packet(const std::string &message)
{
    std::vector<std::string> fragments;
    uint16_t message_size = message.size();
    uint16_t num_fragments = (message_size + MAX_HEADER_SIZE) / MTU;
    uint16_t remaining_bytes = (message_size + MAX_HEADER_SIZE) % MTU;
    uint16_t offset = 0;
    for (uint16_t i = 0; i < num_fragments; i++)
    {
        std::string fragment = message.substr(offset, MTU);
        fragments.push_back(fragment);
        offset += MTU;
    }
    if (remaining_bytes > 0)
    {
        std::string fragment = message.substr(offset, remaining_bytes);
        fragments.push_back(fragment);
    }
    return fragments;
}

// int main()
// {
//     Packets packets(1500, 1);
//     std::cout << "Packets class created" << std::endl;
//     std::string message(1500, 'a');

//     std::vector<std::string> packets_list = packets.get_packets(2, message);

//     for (const auto &packet : packets_list)
//     {
//         std::cout << packet.c_str() << std::endl;
//     }

//     simdjson::dom::parser parser;
//     // std::ifstream data_stream(packets_list[0].c_str());
//     simdjson::dom::element doc = parser.parse(packets_list[1]);
//     std::cout << doc["message"] << std::endl; // prints Hello World!

//     auto header = doc["header"];
//     std::cout<<"Header Id: " << header["id"] << std::endl;

//     std::cout<< "code breks here"<<std::endl;
//     int64_t id;
//     id = header["id"];
//     std::cout<< id <<std::endl;

//     return 0;
// }
