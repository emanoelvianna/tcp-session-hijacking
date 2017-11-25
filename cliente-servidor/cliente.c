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

/* declaracao das constantes */
#define IP_SERVIDOR "::1"
#define PORTA_SERVIDOR 4242 /* definindo a porta de conexão  */
#define TAMANHO_MENSAGEM 100 /* definicao do tamanho da mensagem */

int main(int argc, char *argv[]) {

    struct sockaddr_in6 servidor;
    int socket_cliente;
    int tamanho = sizeof(servidor);
    char mensagem[TAMANHO_MENSAGEM];

    /* abrindo o socket com IPv6 */
    if ((socket_cliente = socket(AF_INET6, SOCK_STREAM, 0)) == -1) {
        perror("[INFO] erro ao abrir socket no cliente!\n");
        return EXIT_FAILURE;
    }
    printf("[INFO] socket do cliente criando com sucesso!\n");

	/* setando o tipo de endeço para comunicacao */
    servidor.sin6_family = AF_INET6;
    /* setando a porta de comunicacao */
    servidor.sin6_port = htons(PORTA_SERVIDOR);
    /* setando um endereço endereço IPv6 servidor */
	inet_pton(AF_INET6, IP_SERVIDOR, &servidor.sin6_addr);

    /* Tries to connect to the servidor */
    if (connect(socket_cliente, (struct sockaddr*) &servidor, tamanho) == -1) {
        perror("[INFO] erro ao conectar ao servidor!\n");
        return EXIT_FAILURE;
    }

    do {
        /* garantindo que não ira existir lixo na mensagem */
        memset(mensagem, 0x0, TAMANHO_MENSAGEM);

        fprintf(stdout, "[INFO] Enviar mensagem ao servidor: ");
        fgets(mensagem, TAMANHO_MENSAGEM, stdin);

        /* enviando pacote ao servidor */
        send(socket_cliente, mensagem, strlen(mensagem), 0);
    } while (strcmp(mensagem, "tchau"));

    /* fechando conexao com o servidor */
    close(socket_cliente);
	fprintf(stdout, "[INFO]Conexao fechada!\n");
	
    return EXIT_SUCCESS;
}
