/* dependencias e utilitarios */
#include "estrutura.h"

/* declaração das variaveis genericas */
uint8_t *pacote;
int i;
int socketDescriptor;
struct sockaddr_ll to;
int operation;
char *url, *path;
char *input_ifname;
u_int16_t fin, syn, rst, ack, window;
u_int32_t seq, ack_seq;
int isEchoRequest;
u_int32_t icmpv6Seq;
char *sourceIP, *destinationIP;

/* declaração das variaives utilizadas no Ethernet */
unsigned int ethernetHeaderLength;
struct ifreq ifr;
char *interface;

unsigned char sourceMacAddress[ETHERNET_ADDR_LEN];
unsigned char destinationMacAddress[ETHERNET_ADDR_LEN];

/* declaração das variaives utilizadas no IPv6 */
struct ip6_hdr *ipv6Header; // a struct ip esta descrita no /usr/include/netinet/ip.h
unsigned int ipv6HeaderLength;

/* declaração das variaveis utilizadas no ICMPv6 */
struct icmp6_hdr *icmpv6Header;
char *icmpv6Data;
unsigned int icmpv6HeaderLength;

/* declaração das variaveis utilizadas no TCP */
unsigned int tcpHeaderLength;
unsigned int tcpDataLength;

void monta_pacote_ipv6()
{
    ipv6Header = (struct ip6_hdr *)&pacote[ethernetHeaderLength];

    ipv6Header->ip6_flow = htonl((6 << 28) | (0 << 20) | 0);                            // IPv6 version (4 bits), Traffic class (8 bits), Flow label (20 bits)
    ipv6Header->ip6_plen = htons(icmpv6HeaderLength + tcpHeaderLength + tcpDataLength); // Payload length - 2 bytes
    ipv6Header->ip6_nxt = IPPROTO_ICMPV6;
    ipv6Header->ip6_hops = 255;

    int status;
    if ((status = inet_pton(AF_INET6, sourceIP, &(ipv6Header->ip6_src))) != 1)
    {
        fprintf(stderr, "inet_pton() failed.\nError message: %s", strerror(status));
        exit(EXIT_FAILURE);
    }
    if ((status = inet_pton(AF_INET6, destinationIP, &(ipv6Header->ip6_dst))) != 1)
    {
        fprintf(stderr, "inet_pton() failed.\nError message: %s", strerror(status));
        exit(EXIT_FAILURE);
    }
}

void monta_header_tcp()
{
    struct tcphdr *tcpHeader; // a struct tcp esta descrita em /usr/include/netinet/tcp.h
    tcpHeader = (struct tcphdr *)&pacote[ethernetHeaderLength + ipv6HeaderLength + icmpv6HeaderLength];

    tcpHeader->source = htons((uint16_t)60000);
    tcpHeader->dest = htons((uint16_t)60000);
    tcpHeader->seq = htonl(seq);
    tcpHeader->ack_seq = htonl(ack_seq);

    tcpHeader->res1 = 0;
    tcpHeader->doff = 5;
    tcpHeader->fin = fin;
    tcpHeader->syn = syn;
    tcpHeader->rst = rst;
    tcpHeader->psh = 1; //PUSH = 1
    tcpHeader->ack = ack;
    tcpHeader->urg = 0;
    tcpHeader->res2 = 0;

    tcpHeader->window = htons(window);
    tcpHeader->check = htons(in_cksum((uint16_t *)tcpHeader, tcpHeaderLength + tcpDataLength)); //htons((uint16_t) 0);
    tcpHeader->urg_ptr = htons((uint16_t)0);
}

void monta_data_tcp()
{
    //TODO:
}

uint16_t icmp6_checksum(struct ip6_hdr iphdr, struct icmp6_hdr icmp6hdr, uint8_t *payload, int payloadlen)
{
    char buf[ethernetHeaderLength + ipv6HeaderLength + icmpv6HeaderLength + tcpHeaderLength + tcpDataLength];
    char *ptr;
    int chksumlen = 0;
    int i;

    ptr = &buf[0]; // ptr points to beginning of buffer buf

    // Copy source IP address into buf (128 bits)
    memcpy(ptr, &iphdr.ip6_src.s6_addr, sizeof(iphdr.ip6_src.s6_addr));
    ptr += sizeof(iphdr.ip6_src);
    chksumlen += sizeof(iphdr.ip6_src);

    // Copy destination IP address into buf (128 bits)
    memcpy(ptr, &iphdr.ip6_dst.s6_addr, sizeof(iphdr.ip6_dst.s6_addr));
    ptr += sizeof(iphdr.ip6_dst.s6_addr);
    chksumlen += sizeof(iphdr.ip6_dst.s6_addr);

    // Copy Upper Layer Packet length into buf (32 bits).
    // Should not be greater than 65535 (i.e., 2 bytes).
    *ptr = 0;
    ptr++;
    *ptr = 0;
    ptr++;
    *ptr = (icmpv6HeaderLength + payloadlen) / 256;
    ptr++;
    *ptr = (icmpv6HeaderLength + payloadlen) % 256;
    ptr++;
    chksumlen += 4;

    // Copy zero field to buf (24 bits)
    *ptr = 0;
    ptr++;
    *ptr = 0;
    ptr++;
    *ptr = 0;
    ptr++;
    chksumlen += 3;

    // Copy next header field to buf (8 bits)
    memcpy(ptr, &iphdr.ip6_nxt, sizeof(iphdr.ip6_nxt));
    ptr += sizeof(iphdr.ip6_nxt);
    chksumlen += sizeof(iphdr.ip6_nxt);

    // Copy ICMPv6 type to buf (8 bits)
    memcpy(ptr, &icmp6hdr.icmp6_type, sizeof(icmp6hdr.icmp6_type));
    ptr += sizeof(icmp6hdr.icmp6_type);
    chksumlen += sizeof(icmp6hdr.icmp6_type);

    // Copy ICMPv6 code to buf (8 bits)
    memcpy(ptr, &icmp6hdr.icmp6_code, sizeof(icmp6hdr.icmp6_code));
    ptr += sizeof(icmp6hdr.icmp6_code);
    chksumlen += sizeof(icmp6hdr.icmp6_code);

    // Copy ICMPv6 ID to buf (16 bits)
    memcpy(ptr, &icmp6hdr.icmp6_id, sizeof(icmp6hdr.icmp6_id));
    ptr += sizeof(icmp6hdr.icmp6_id);
    chksumlen += sizeof(icmp6hdr.icmp6_id);

    // Copy ICMPv6 sequence number to buff (16 bits)
    memcpy(ptr, &icmp6hdr.icmp6_seq, sizeof(icmp6hdr.icmp6_seq));
    ptr += sizeof(icmp6hdr.icmp6_seq);
    chksumlen += sizeof(icmp6hdr.icmp6_seq);

    // Copy ICMPv6 checksum to buf (16 bits)
    // Zero, since we don't know it yet.
    *ptr = 0;
    ptr++;
    *ptr = 0;
    ptr++;
    chksumlen += 2;

    // Copy ICMPv6 payload to buf
    memcpy(ptr, payload, payloadlen * sizeof(uint8_t));
    ptr += payloadlen;
    chksumlen += payloadlen;

    // Pad to the next 16-bit boundary
    for (i = 0; i < payloadlen % 2; i++, ptr++)
    {
        *ptr = 0;
        ptr += 1;
        chksumlen += 1;
    }

    return in_cksum((uint16_t *)buf, chksumlen);
}

unsigned short in_cksum(unsigned short *addr, int len)
{
    register int sum = 0;
    u_short answer = 0;
    register u_short *w = addr;
    register int nleft = len;

    /*
     * Our algorithm is simple, using a 32 bit accumulator (sum), we add
     * sequential 16 bit words to it, and at the end, fold back all the
     * carry bits from the top 16 bits into the lower 16 bits.
     */
    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    /* mop up an odd byte, if necessary */
    if (nleft == 1)
    {
        *(u_char *)(&answer) = *(u_char *)w;
        sum += answer;
    }

    /* add back carry outs from top 16 bits to low 16 bits */
    sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
    sum += (sum >> 16);                 /* add carry */
    answer = ~sum;                      /* truncate to 16 bits */
    return (answer);
}

int main(int argc, char **argv)
{
    return 0;
}