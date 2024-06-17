#ifndef PACKETS_H
#define PACKETS_H

#include <string>

// Example packet structures
struct PacketHeader
{
    uint16_t type;       // Packet type identifier
    uint16_t length;     // Length of the packet data
    uint16_t sequence;   // Sequence number
    uint16_t message_id; // Message ID
};

struct ChatMessagePacket
{
    PacketHeader header;
    std::string message;
};

struct ackPacket
{
    PacketHeader header;
    uint16_t ack; // The message ID of the message being acknowledged
};

struct PlayerMovePacket
{
    PacketHeader header;
    // TODO: Add THE PLAYER CARD SELECTED
};


#endif // PACKETS_H
