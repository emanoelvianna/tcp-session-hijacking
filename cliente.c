#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ether.h>
/* utilizando os utilitarios */
#include "estrutura.h"

int main(int argc, char *argv[])
{
	int raw6_socket;

	/* abrindo o socket com IPv6 */
	raw6_socket = socket(PF_PACKET, SOCK_RAW, IPPROTO_TCP);
	if (raw6_socket < 0)
	{
		fprintf(stderr, "[INFO] erro ao abrir socket no servidor!\n");
		exit(1);
	}

	return 0;
}