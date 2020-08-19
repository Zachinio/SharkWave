#include <cstdio>
#include <string>
#include <dlfcn.h>
#include <logger/Logger.hpp>
#include "utils/NetworkUtils.h"
#include "PcapDefines.h"
#include "utils/ShellUtils.h"
#include "PacketType.h"

void showUsage();

bool isPidOwner(u_int port, PacketType type);

void startSniffing();

using std::string;
using std::stoi;
using std::to_string;

int pid = -1;
int packetsCount = 10000;
char *device;
string package;
string localIp;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        showUsage();
        return 0;
    }
    /* TODO handle empty arguments */
    for (int i = 1; i < argc; i += 2) {
        if (i + 1 == argc) {
            printf("no value passed for %s\n", argv[i]);
            return -1;
        }
        char *parameter = argv[i];
        char *value = argv[i + 1];

        if (strcmp("--pid", parameter) == 0) {
            pid = stoi(value);
        } else if (strcmp("--device", parameter) == 0) {
            device = value;
        } else if (strcmp("--count", parameter) == 0) {
            packetsCount = stoi(value);
        } else if (strcmp("--package", parameter) == 0) {
            package = string(value);
        } else {
            printf("Unknown parameter %s\n", parameter);
            return -2;
        }
    }

    try {
        Logger::Log("------------------- Start of Log -------------------\n");
        startSniffing();
    }
    catch (const std::runtime_error &err) {
        Logger::Log("error" + string(err.what()));
    }

    Logger::Log("------------------- End of Log -------------------\n");
    return 0;
}

bool isPidOwner(u_int port, PacketType type) {
    string param = getParamForType(type);
    int offset = getOffsetByType(type);
    string command = "ss -pn" + param + " | grep 'pid=" + to_string(pid) + "'";

    if (package.compare("") != 0) {
        package = package.size() > 15 ? package.substr(15, package.size() - 15) : package;
        command = "ss -pn" + param + " | grep '\"" + package + "\"'";
    }


    const string &ss = executeCommand(command);

    std::stringstream stream(ss);
    std::vector<std::string> result{std::istream_iterator<std::string>(stream), {}};
    if (result.size() < offset) {
        return false;
    }

    for (int i = 0; i + offset - 2 < result.size(); i += offset) {
        string address = result[i + offset - 3];
        int colonIndex = address.find_last_of(':');
        if (colonIndex > 0) {
            string portFound = address.substr(colonIndex + 1, address.size() - colonIndex);
            try {
                int portFoundInt = stoi(portFound);
                if (port == portFoundInt) {
                    return true;
                }
            } catch (runtime_error &err) {

            }
        }
    }
    return false;
}

void gotPacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    const struct ether_header *ethernetHeader;
    const struct ip *ipHeader;
    const struct tcphdr *tcpHeader;
    const struct udphdr *udpHeader;
    char sourceIP[INET_ADDRSTRLEN];
    char destIP[INET_ADDRSTRLEN];
    u_int sourcePort = 0, destPort;
    PacketType type = PacketType::tcp;
    u_char *data;

    ethernetHeader = (struct ether_header *) packet;
    if (ntohs(ethernetHeader->ether_type) == ETHERTYPE_IP) {

        ipHeader = (struct ip *) (packet + sizeof(struct ether_header));
        inet_ntop(AF_INET, &(ipHeader->ip_src), sourceIP, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ipHeader->ip_dst), destIP, INET_ADDRSTRLEN);
        if (strcmp(sourceIP, localIp.c_str()) != 0) {
            return; /*Not a packet from the device */
        }
        printf("--------------------\n");

        if (ipHeader->ip_p == IPPROTO_TCP) {
            tcpHeader = (struct tcphdr *) (packet + sizeof(struct ether_header) +
                                           sizeof(struct ip));
            sourcePort = ntohs(tcpHeader->source);
            destPort = ntohs(tcpHeader->dest);
            data = (u_char *) (packet + sizeof(struct ether_header) + sizeof(struct ip) +
                               sizeof(struct tcphdr));
        } else if (ipHeader->ip_p == IPPROTO_UDP) {
            type = PacketType::udp;
            udpHeader = (struct udphdr *) (packet + sizeof(struct ether_header) +
                                           sizeof(struct ip));
            sourcePort = ntohs(udpHeader->source);
            destPort = ntohs(udpHeader->dest);
        } else if (ipHeader->ip_p == IPPROTO_ICMP) {

        }
        printf("packet type: %s\n", getParamForType(type).c_str());
        printf("sourcePort: %d,destPort %d\n", sourcePort, destPort);
        printf("packet owned by %d: %s", pid, isPidOwner(sourcePort, type) ? "true" : "false");
        printf("--------------------\n");
    }
}

void startSniffing() {
    void *libpcap = dlopen("libpcap.so", RTLD_NOW);
    char errbuf[PCAP_ERRBUF_SIZE];

    if (nullptr == device) {
        device = (char *) findDevice(libpcap).c_str();
    }
    localIp = getLocalIpAdress();
    Logger::Log("device is: " + string(device));

    auto *pcap_open_live = (pcap_open_live_function) dlsym(libpcap, "pcap_open_live");
    auto *pcap_loop = (pcap_loop_function) dlsym(libpcap, "pcap_loop");
    auto pcap_close = (pcap_close_function) dlsym(libpcap, "pcap_close");

    pcap_t *handle = pcap_open_live(device, BUFSIZ, 1, 1000, errbuf);
    pcap_loop(handle, packetsCount, gotPacket, nullptr);

    pcap_close(handle);
    dlclose(libpcap);
}

void showUsage() {
    printf("Usage of SharkWave\n");
    printf("--pid <PID>\n   A pid to sniff its network\n");
    printf("--device <device interface>\n   The device name to sniff\n");
    printf("--count <count>\n   The number of packets to count,default is 10000\n");
    printf("--package <package>\n   the package name to sniff\n");
}
