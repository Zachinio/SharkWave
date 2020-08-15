#include <sys/types.h>
#include <memory>

using std::unique_ptr;
using std::ref;
using std::make_unique;

const int PCAP_ERRBUF_SIZE = 1024;

struct pcap_pkthdr {
    struct timeval ts;    /* time stamp */
    u_int caplen;    /* length of portion present */
    u_int len;    /* length of this packet (off wire) */
};

typedef void (pcap_handler)(unsigned char *, const pcap_pkthdr *, const unsigned char *);

typedef void * pcap_t;

typedef char *(*pcap_lookupdev_function)(char **);

typedef pcap_t *(*pcap_open_live_function)(char *device, int snaplen, int promisc, int to_ms,
                                           char *errbuf);

typedef int (*pcap_loop_function)(pcap_t *handle, int count, pcap_handler *callback, u_char *user);


string & findDevice(void *libpcap) {
    char *errbuf[PCAP_ERRBUF_SIZE];
    auto pcap_lookupdev = (pcap_lookupdev_function) dlsym(libpcap, "pcap_lookupdev");
    string device =  string(pcap_lookupdev(errbuf));
    return ref(device);
}