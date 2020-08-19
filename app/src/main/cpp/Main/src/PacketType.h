#include <string>


enum PacketType {
    tcp, udp
};

string getParamForType(PacketType type) {
    if (type == PacketType::tcp) {
        return "t";
    }
    if (type == PacketType::udp) {
        return "u";
    }
    return "a";
}

int getOffsetByType(PacketType type) {
    if (type == PacketType::tcp) {
        return 6;
    }
    if (type == PacketType::udp) {
        return 5;
    }
    return 6;
}