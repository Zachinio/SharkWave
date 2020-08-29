#include <cstdio>
#include <string>
#include <dlfcn.h>
#include <logger/Logger.hpp>
#include "utils/NetworkUtils.h"
#include "PcapDefines.h"
#include "utils/ShellUtils.h"
#include "PacketType.h"

void showUsage();

bool isPidOwner(PacketType type, u_int sPort, u_int dPort, char *sIp, char *dIp);

void startSniffing();

void printPacketInfo(char sourceIP[16], u_int sourcePort, char destIP[16], u_int destPort,PacketType type);

using std::string;
using std::stoi;
using std::to_string;

char *pid;
char *device;
int packetsCount = 10000;

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
            printf("no value passed\n");
            return -1;
        }
        char *parameter = argv[i];
        char *value = argv[i + 1];

        if (strcmp("--pid", parameter) == 0) {
            pid = value;
        } else if (strcmp("--device", parameter) == 0) {
            device = value;
        } else if (strcmp("--count", parameter) == 0) {
            packetsCount = stoi(value);
        } else if (strcmp("--package", parameter) == 0) {
            package = string(value);
            string pidFound = executeCommand("pidof " + package);
            if (pidFound.empty()) {
                printf("Couldn't find a pid for this package\n");
                return -1;
            }
            pid = (char *) pidFound.substr(0, pidFound.length() - 1).c_str();
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

bool isPidOwner(PacketType type, u_int sPort, u_int dPort, char *sIp, char *dIp) {
    string protocolName = getProtoclName(type);
    int offset = getOffsetByType(type);
    string command = string("cat /proc/")
            .append(pid)
            .append("/net/nf_conntrack | grep ")
            .append(protocolName)
            .append(" | grep src=")
            .append(sIp)
            .append(" | grep dst=")
            .append(dIp);
    if (PacketType::icmp != type) {
        command.append(" | grep sport=")
                .append(to_string(sPort))
                .append(" | grep dport=")
                .append(to_string(dPort));
    }
    const string &networkInfo = executeCommand(command);

    std::stringstream stream(networkInfo);
    std::vector<std::string> result{std::istream_iterator<std::string>(stream), {}};
    return result.size() >= offset;
}

void gotPacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    const struct ether_header *ethernetHeader;
    const struct ip *ipHeader;
    char sourceIP[INET_ADDRSTRLEN];
    char destIP[INET_ADDRSTRLEN];
    u_int sourcePort = 0, destPort = 0;
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
        if (ipHeader->ip_p == IPPROTO_TCP) {
            const struct tcphdr *tcpHeader;
            tcpHeader = (struct tcphdr *) (packet + sizeof(struct ether_header) +
                                           sizeof(struct ip));
            sourcePort = ntohs(tcpHeader->source);
            destPort = ntohs(tcpHeader->dest);
            data = (u_char *) (packet + sizeof(struct ether_header) + sizeof(struct ip) +
                               sizeof(struct tcphdr));
        } else if (ipHeader->ip_p == IPPROTO_UDP) {
            type = PacketType::udp;
            const struct udphdr *udpHeader;
            udpHeader = (struct udphdr *) (packet + sizeof(struct ether_header) +
                                           sizeof(struct ip));
            sourcePort = ntohs(udpHeader->source);
            destPort = ntohs(udpHeader->dest);
        } else if (ipHeader->ip_p == IPPROTO_ICMP) {
            type = PacketType::icmp;
        }
        if (isPidOwner(type, sourcePort, destPort, sourceIP, destIP)) {
            printPacketInfo(sourceIP, sourcePort, destIP, destPort,type);
        }
    }
}

void printPacketInfo(char *sourceIP, u_int sourcePort, char *destIP, u_int destPort,PacketType type) {
    printf("--------------------\n");
    printf("%s\n",getProtoclName(type).c_str());
    printf("source:\n");
    printf("address: %s\n", sourceIP);
    printf("port: %d\n", sourcePort);

    printf("destination:\n");
    printf("address: %s\n", destIP);
    printf("port: %d\n", destPort);

    printf("process/package: %s\n", package.empty() ? pid : package.c_str());
    printf("--------------------\n");

}

void startSniffing() {
    void *libpcap = dlopen("libpcap.so", RTLD_NOW);
    char errbuf[PCAP_ERRBUF_SIZE];

    if (nullptr == device) {
        device = (char *) findDevice(libpcap).c_str();
    }
    localIp = getLocalIpAddress(device);
    Logger::Log("device is: " + string(device) + "addr: " + localIp + ",pid: " + string(pid));

    auto *pcap_open_live = (pcap_open_live_function) dlsym(libpcap, "pcap_open_live");
    auto *pcap_loop = (pcap_loop_function) dlsym(libpcap, "pcap_loop");
    auto pcap_close = (pcap_close_function) dlsym(libpcap, "pcap_close");

    pcap_t *handle = pcap_open_live(device, BUFSIZ, 1, 1000, errbuf);
    if (nullptr == handle) {
        printf("network device is not ready\n");
        return;
    }
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
