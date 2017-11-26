/* dependencias e utilitarios */
#include "estrutura.h"

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
    char *tcpData;
    tcpData = (char *)malloc(IP_MAXPACKET * sizeof(char));
    memset(tcpData, 0, IP_MAXPACKET * sizeof(char));

    if (operation != ZERO_DATA)
    {
        if (operation == GET)
        {
            sprintf(tcpData, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", path, url);
        }
        else if (operation == HTML_DATA)
        {
            sprintf(tcpData, "%s", url);
        }

        tcpDataLength = strlen(tcpData);
        memcpy((pacote + ethernetHeaderLength + ipv6HeaderLength + icmpv6HeaderLength + tcpHeaderLength), tcpData, tcpDataLength * sizeof(uint8_t));
    }
    else if (operation == ZERO_DATA)
    {
        tcpDataLength = 0;
    }
}

int main(int argc, char **argv)
{
}