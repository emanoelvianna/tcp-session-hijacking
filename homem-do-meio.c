/* dependencia externa*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <linux/if_packet.h>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#include <netdb.h>
#include <netinet/in.h>
#include <bits/ioctls.h>
#include <linux/if_ether.h>
/* dependencia de utilitarios */
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
u_int8_t destinationMacAddress[ETH_ALEN];

/* declaração das variaives utilizadas no Ethernet */
unsigned int ethernetHeaderLength;
struct ifreq ifr;
char *interface;
uint8_t *sourceMacAddress;

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

int obter_mac()
{
	int fd;
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, input_ifname, IFNAMSIZ - 1);
	if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0)
	{
		perror("[INFO] falha ao capturar MAC de origem!\n");
		exit(EXIT_FAILURE);
	}
	close(fd);

	strcpy(sourceMacAddress, ifr.ifr_hwaddr.sa_data);

	return 0;
}

void monta_pacote_ethernet()
{
	struct ether_header *ethernetHeader;
	ethernetHeader = (struct ether_header *)&pacote[0];

	for (i = 0; i < 6; i++)
	{
		ethernetHeader->ether_shost[i] = sourceMacAddress[i];
	}

	ethernetHeader->ether_type = htons(ETH_P_IPV6);
	for (i = 0; i < ETH_ALEN; i++)
	{
		ethernetHeader->ether_dhost[i] = destinationMacAddress[i];
	}
}

void monta_pacote_ipv6()
{
	ipv6Header = (struct ip6_hdr *)&pacote[ethernetHeaderLength];

	ipv6Header->ip6_flow = htonl((6 << 28) | (0 << 20) | 0);							// IPv6 version (4 bits), Traffic class (8 bits), Flow label (20 bits)
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

void monta_pacote_reply_icmpv6()
{
	icmpv6Header = (struct icmp6_hdr *)&pacote[ethernetHeaderLength + ipv6HeaderLength];
	icmpv6Header->icmp6_type = (ICMP6_ECHO_REPLY);
	icmpv6Header->icmp6_code = 0;
	icmpv6Header->icmp6_id = htons(666); //Identificador
	icmpv6Header->icmp6_seq = htons(icmpv6Seq);
	icmpv6Header->icmp6_cksum = (icmp6_checksum(*ipv6Header, *icmpv6Header, &pacote[ethernetHeaderLength + ipv6HeaderLength + icmpv6HeaderLength], tcpHeaderLength + tcpDataLength));
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
	sum += (sum >> 16);					/* add carry */
	answer = ~sum;						/* truncate to 16 bits */
	return (answer);
}

/* informando os parametros esperado */
void usar(char *exec)
{
	printf("%s <interface de rede> <MAC destino>\n", exec);
}

int main(int argc, char **argv)
{
	/* verificando a quantidade de parametros */
	if (argc < 3)
	{
		usar(argv[0]);
	}

	/* obtendo interface de rede */
	input_ifname = argv[1];
	/* obtendo o mac do servidor */
	obter_mac();

	/*  */

	pacote = (uint8_t *)malloc(IP_MAXPACKET * sizeof(uint8_t));
	memset(pacote, 0, IP_MAXPACKET * sizeof(uint8_t));

	/* definindo tamanhos para os pacotes */
	ethernetHeaderLength = 14;
	ipv6HeaderLength = 40;
	icmpv6HeaderLength = 8;
	tcpHeaderLength = sizeof(struct tcphdr);

	monta_pacote_ethernet();
	monta_pacote_ipv6();
	monta_pacote_reply_icmpv6();

	int retValue = 0;
	if ((retValue = sendto(socketDescriptor, (char *)pacote, ethernetHeaderLength + ipv6HeaderLength + icmpv6HeaderLength + tcpHeaderLength + tcpDataLength, 0, (struct sockaddr *)&to, sizeof(to)) < 0))
	{
		printf("[INFO] erro ao enviar pacote ICMP: %d\n", retValue);
		exit(1);
	}

	return 0;
}