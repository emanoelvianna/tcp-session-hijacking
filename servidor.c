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

int main(int argc, char *argv[])
{
	estrutura_pacote pacote;
	int socket_servidor;
	struct sockaddr_in6 servidor;

	/* abrindo o socket com IPv6 */
	socket_servidor = socket(PF_INET6, SOCK_RAW, IPPROTO_TCP);
	if (socket_servidor < 0)
	{
		fprintf(stderr, "[INFO] erro ao abrir socket no servidor!\n");
		exit(1);
	}
	printf("[INFO] socket do servidor criando com sucesso!\n");

	/* setando o tipo de endeço para comunicacao */
	servidor.sin6_family = AF_INET6;
	/* setando a porta de comunicacao */
	servidor.sin6_port = htons(PORT);
	/* setando um endereço endereço IPv6 qualquer */
	inet_pton(AF_INET6, "2001:db8:8714:3a90::12", &servidor.sin6_addr);

	/* associando o socket a porta */
	if (bind(socket_servidor, (const void *)&servidor, sizeof(servidor)) == -1)
	{
		perror("[INFO] erro ao tentar associar a porta!\n");
		return EXIT_FAILURE;
	}

	printf("[INFO] esperando cliente enviar mensagem na conexao!\n");
	/* enquanto cliente enviar mensagem */
	do
	{
		/* garantindo que não ira existir lixo no pacote */
		memset(&pacote, 0x00, sizeof(pacote));
		/* recebendo mensagens do cliente */
		recv(socket_servidor, (char *)&pacote, sizeof(pacote), 0x0);

		if (pacote.source_port == PORT)
		{
			/* garantindo que não ira existir lixo no pacote */
			memset(&pacote, 0x00, sizeof(pacote));
			/* criando trecho do pacote Ethernet */
			memcpy(argv[1], pacote.source_ethernet_address, ETHERNET_ADDR_LEN);
			/* criando trecho do pacote IPv6 */
			/* criando trecho do pacote TCP */

			/* enviando pacote ao cliente */
			if (send(socket_servidor, &pacote, sizeof(pacote), 0) < 0)
			{
				perror("[INFO] nao é possível pacote ao servidor!\n");
			}

			printf("[INFO] cliente diz: %s\n", pacote.mensagem);
			if (strcmp(pacote.mensagem, "tchau") == 0)
			{
				printf("[INFO] cliente acabou de sair da conexao!\n");
			}
		}
	} while (strcmp(pacote.mensagem, "tchau"));

	/* fechando conexao com servidor e cliente */
	close(socket_servidor);

	return 0;
}