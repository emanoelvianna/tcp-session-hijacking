/* dependencia externa*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
/* dependencia de utilitarios */
#include "estrutura.h"

char *input_ifname;
unsigned char mac_local[ETHERNET_ADDR_LEN];

/* Obtem o endereco MAC da interface local */
void getMac()
{
	int fd;
	struct ifreq ifr;

	fd = socket(AF_INET6, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = AF_INET6;
	strncpy(ifr.ifr_name, input_ifname, IFNAMSIZ - 1);
	ioctl(fd, SIOCGIFHWADDR, &ifr);
	close(fd);

	strcpy(mac_local, ifr.ifr_hwaddr.sa_data);
}

/** realizando um requisicao na rede **/
void *requisicao()
{
	int arp_socket, ifreq_socket, optval;
	estrutura_pacote_icmp pacote;
	struct ifreq ifr;
	struct sockaddr_in6 sa;

	/* Cria um descritor de socket do tipo RAW */
	if ((ifreq_socket = socket(AF_INET6, SOCK_RAW, htons(ETH_P_ALL))) == -1)
	{
		perror("Erro ao tentar criar o socket!");
		return (void *)-1;
	}

	/* Obtem o endereco IP da interface local */
	if (ioctl(ifreq_socket, SIOCGIFADDR, &ifr) < 0)
	{
		perror("ERROR ao tentar obter IP de origem!");
		return (void *)-1;
	}
}

int main(int argc, char *argv[]) {

}