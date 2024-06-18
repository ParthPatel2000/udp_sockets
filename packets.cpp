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
            buffer = header_JSON(fragment.size(), packetType, fragments.size(), num_fragments++) + "\n";
            buffer += message_JSON(fragment);
            packets.push_back(buffer);
            packet_buffer[packet_id] = buffer;
            break;
        default:
            break;
        }
    }
    return packets;
}

std::string Packets::create_packet(std::vector<std::string> message_JSON)
{
    std::string packet;
    packet += message_JSON[0];
    packet += "\n";
    packet += message_JSON[1];
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

//Remove packet from buffer after getting ack
void Packets::remove_packet(uint16_t id)
{
    packet_buffer.erase(id);
}

//Fetch packet from buffer if ack is not received to resend
std::string Packets::fetch_packet(uint16_t id)
{
    return packet_buffer[id];
}

// Header in JSON format
std::string Packets::header_JSON(uint16_t size, uint16_t type, uint16_t fragments = 0, uint16_t fragment_seq = 0)
{
    std::stringstream ss;
    ss << R"({header:{)"
       << R"(size:)" << size
       << R"(,type:)" << type
       << R"(,fragments:)" << fragments
       << R"(,fragment_seq)" << fragment_seq
       << R"(,id:)" << packet_id
       << "}}";
    return ss.str();
}

// Message in JSON format
std::string Packets::message_JSON(const std::string &message)
{
    std::stringstream ss;
    ss << R"({message:")" << message
       << R"("})";
    return ss.str();
}

std::vector<std::string> Packets::fragment_packet(const std::string &message)
{
    std::vector<std::string> fragments;
    uint16_t message_size = message.size();
    uint16_t num_fragments = message_size / MTU;
    uint16_t remaining_bytes = message_size % MTU;
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

int main()
{
    Packets packets;
    std::cout << "Packets class created" << std::endl;

    
    return 0;
}
