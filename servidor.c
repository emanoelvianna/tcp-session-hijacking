/* dependencia externa*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
/* dependencia de utilitarios */
#include "estrutura.h"

unsigned short calcular_checkSum(unsigned short *buf, int len)
{
	unsigned long sum;
	for (sum = 0; len > 0; len--)
		sum += *buf++;
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return (unsigned short)(~sum);
}

int main(int argc, char *argv[])
{
	int raw6_socket;
	struct sockaddr_in sin, din;
	estrutura_pacote pacote;

	/* abrindo o socket com IPv6 */
	raw6_socket = socket(AF_INET6, SOCK_RAW, IPPROTO_TCP);
	if (raw6_socket < 0)
	{
		fprintf(stderr, "[INFO] erro ao abrir socket no servidor!\n");
		exit(1);
	}
	/* setando o tipo de endeço para comunicacao */
	sin.sin_family = AF_INET;
	din.sin_family = AF_INET;
	/* setando a porta de comunicacao */
	sin.sin_port = htons(atoi(argv[2]));
	din.sin_port = htons(atoi(argv[4]));
	/* setando o endereço de origem e destino */
	sin.sin_addr.s_addr = inet_addr(argv[1]);
	din.sin_addr.s_addr = inet_addr(argv[3]);

	printf("[INFO] enviando um pacote TCP!\n");
	if (sendto(raw6_socket, &pacote, sizeof(pacote), 0, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		printf("[INFO] erro ao enviear o pacote TCP!\n");
		exit(-1);
	}
	else
	{
		printf("[INFO] pacote TCP enviado!\n");
	}
	return 0;
}