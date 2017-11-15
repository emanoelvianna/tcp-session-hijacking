/* dependencia externa*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <unistd.h>
/* dependencia de utilitarios */
#include "estrutura.h"

//TODO: deve ser informado como parametro!
#define IP_SERVIDOR "::1"

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
	estrutura_pacote pacote;
	int socket_cliente;
	struct sockaddr_in6 cliente;

	/* abrindo o socket com IPv6 */
	socket_cliente = socket(AF_INET6, SOCK_RAW, IPPROTO_TCP);
	if (socket_cliente < 0)
	{
		fprintf(stderr, "[INFO] erro ao abrir socket no servidor!\n");
		exit(1);
	}
	printf("[INFO] socket do cliente criando com sucesso!\n");

	/* setando o tipo de endeço para comunicacao */
	cliente.sin6_family = AF_INET6;
	/* setando a porta de comunicacao */
	cliente.sin6_port = htons(PORT);
	/* setando um endereço endereço IPv6 qualquer */
	inet_pton(AF_INET6, "::1", &cliente.sin6_addr);

	do
	{
		/* garantindo que não ira existir lixo no pacote */
		memset(&pacote, 0x00, sizeof(pacote));

		fprintf(stdout, "Enviar mensagem ao servidor: ");
		fgets(pacote.mensagem, TAMANHO_MENSAGEM, stdin);

		/* garantindo que não ira existir lixo no pacote */
		memset(&pacote, 0x00, sizeof(pacote));
		/* criando trecho do pacote Ethernet */
		memcpy(argv[1], pacote.source_ethernet_address, ETHERNET_ADDR_LEN);
		/* criando trecho do pacote IPv6 */
		/* criando trecho do pacote TCP */

		/* enviando pacote ao servidor */
		if (send(socket_cliente, &pacote, sizeof(pacote), 0) < 0)
		{
			perror("[INFO] nao é possível pacote ao servidor!\n");
		}

	} while (strcmp(pacote.mensagem, "tchau"));

	return 0;
}