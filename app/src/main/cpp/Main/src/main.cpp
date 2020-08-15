#include <cstdio>
#include <string>
#include <dlfcn.h>
#include <logger/Logger.hpp>
#include "PcapDefines.h"

void showUsage();

void startSniffing();

using std::string;
using std::stoi;

int pid = -1;
int packetsCount = 100;
char *device;

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
        } else {
            printf("Unknown parameter %s\n", parameter);
            return -2;
        }
        startSniffing();
    }

    try {
        Logger::Log("------------------- Start of Log -------------------\n");

    }
    catch (const std::runtime_error &err) {
        Logger::Log("error" + string(err.what()));
    }

    Logger::Log("------------------- End of Log -------------------\n");
    return 0;
}

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    printf("packet recevied\n");
}

void startSniffing() {
    void *libpcap = dlopen("libpcap.so", RTLD_NOW);
    char errbuf[PCAP_ERRBUF_SIZE];

    if (nullptr == device) {
        device = (char *)findDevice(libpcap).c_str();
    }
    Logger::Log("device is: " + string(device));

    auto *pcap_open_live = (pcap_open_live_function) dlsym(libpcap, "pcap_open_live");
    auto *pcap_loop = (pcap_loop_function) dlsym(libpcap, "pcap_loop");

    pcap_t *handle = pcap_open_live(device, BUFSIZ, 1, 1000, errbuf);
    pcap_loop(handle, packetsCount, got_packet, nullptr);
}

void showUsage() {
    printf("Usage of SharkWave\n");
    printf("--pid <PID>\n   A pid to sniff its network\n");
    printf("--device <device interface>\n   The device name to sniff\n");
    printf("--count <count>\n   The number of packets to count,default is 10000\n");
}
