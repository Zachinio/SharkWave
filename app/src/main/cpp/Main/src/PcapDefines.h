#include <sys/types.h>
#include <memory>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

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
typedef char *(*pcap_close_function)(pcap_t *);

typedef pcap_t *(*pcap_open_live_function)(char *device, int snaplen, int promisc, int to_ms,
                                           char *errbuf);

typedef int (*pcap_loop_function)(pcap_t *handle, int count, pcap_handler *callback, u_char *user);

/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN	6

/* Ethernet header */
struct sniff_ethernet {
    u_char ether_dhost[ETHER_ADDR_LEN]; /* Destination host address */
    u_char ether_shost[ETHER_ADDR_LEN]; /* Source host address */
    u_short ether_type; /* IP? ARP? RARP? etc */
};

/* IP header */
struct sniff_ip {
    u_char ip_vhl;		/* version << 4 | header length >> 2 */
    u_char ip_tos;		/* type of service */
    u_short ip_len;		/* total length */
    u_short ip_id;		/* identification */
    u_short ip_off;		/* fragment offset field */
#define IP_RF 0x8000		/* reserved fragment flag */
#define IP_DF 0x4000		/* dont fragment flag */
#define IP_MF 0x2000		/* more fragments flag */
#define IP_OFFMASK 0x1fff	/* mask for fragmenting bits */
    u_char ip_ttl;		/* time to live */
    u_char ip_p;		/* protocol */
    u_short ip_sum;		/* checksum */
    struct in_addr ip_src,ip_dst; /* source and dest address */
};
#define IP_HL(ip)		(((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)		(((ip)->ip_vhl) >> 4)

/* TCP header */
typedef u_int tcp_seq;

struct sniff_tcp {
    u_short th_sport;	/* source port */
    u_short th_dport;	/* destination port */
    tcp_seq th_seq;		/* sequence number */
    tcp_seq th_ack;		/* acknowledgement number */
    u_char th_offx2;	/* data offset, rsvd */
#define TH_OFF(th)	(((th)->th_offx2 & 0xf0) >> 4)
    u_char th_flags;
#define TH_FIN 0x01
#define TH_SYN 0x02
#define TH_RST 0x04
#define TH_PUSH 0x08
#define TH_ACK 0x10
#define TH_URG 0x20
#define TH_ECE 0x40
#define TH_CWR 0x80
#define TH_FLAGS (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
    u_short th_win;		/* window */
    u_short th_sum;		/* checksum */
    u_short th_urp;		/* urgent pointer */
};

string & findDevice(void *libpcap) {
    char *errbuf[PCAP_ERRBUF_SIZE];
    auto pcap_lookupdev = (pcap_lookupdev_function) dlsym(libpcap, "pcap_lookupdev");
    string device =  string(pcap_lookupdev(errbuf));
    return ref(device);
}