#include <string>


enum PacketType {
    tcp, udp,icmp
};

string getProtoclName(PacketType type) {
    if (type == PacketType::tcp) {
        return "tcp";
    }
    if (type == PacketType::udp) {
        return "udp";
    }
    if (type == PacketType::icmp) {
        return "icmp";
    }
    return "tcp";
}

int getOffsetByType(PacketType type) {
    if (type == PacketType::tcp) {
        return 18;
    }
    if (type == PacketType::udp) {
        return 16;
    }
    return 16;
}