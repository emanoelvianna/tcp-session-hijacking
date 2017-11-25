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

int main(void)
{

    struct sockaddr_in6 cliente, servidor;
    int socket_servidor, socket_cliente;
    char mensagem[TAMANHO_MENSAGEM];

    fprintf(stdout, "[INFO] Inciando o servidor\n");

    /* abrindo o socket com IPv6 */
    socket_servidor = socket(AF_INET6, SOCK_STREAM, 0);
    if (socket_servidor == -1)
    {
        perror("[INFO] erro ao abrir socket no servidor!\n");
        return EXIT_FAILURE;
    }

    /* setando o tipo de endeço para comunicacao */
    servidor.sin6_family = AF_INET6;
    /* setando a porta de comunicacao */
    servidor.sin6_port = htons(PORTA_SERVIDOR);
    /* setando um endereço endereço IPv6 servidor */
    inet_pton(AF_INET6, "::1", &servidor.sin6_addr);

    /* Handle the error of the port already in use */
    int confirmacao = 1;
    if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &confirmacao, sizeof(int)) == -1)
    {
        perror("[INFO] erro ao setar opções no servidor!\n");
        return EXIT_FAILURE;
    }

    if (bind(socket_servidor, (struct sockaddr *)&servidor, sizeof(servidor)) == -1)
    {
        perror("[INFO] erro ao associar o servidor à porta!\n");
        return EXIT_FAILURE;
    }

    /* aguardando conexao de cliente */
    if (listen(socket_servidor, 1) == -1)
    {
        perror("[INFO] erro durante criar o escuta para o cliente!\n");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "[INFO] servidor ouvindo na porta: %d\n", PORTA_SERVIDOR);

    socklen_t client_len = sizeof(cliente);
    if ((socket_cliente = accept(socket_servidor, (struct sockaddr *)&cliente, &client_len)) == -1)
    {
        perror("[INFO] erro ao aceitar conexao com o cliente!\n");
        return EXIT_FAILURE;
    }

    fprintf(stdout, "[INFO] cliente entrou na conexao!\n");

    /* Communicates with the client until bye message come */
    do {
        /* garantindo que não ira existir lixo na mensagem */
        memset(mensagem, 0x0, TAMANHO_MENSAGEM);

        int message_len;
        if ((message_len = recv(socket_cliente, mensagem, TAMANHO_MENSAGEM, 0)) > 0)
        {
            mensagem[message_len - 1] = '\0';
            printf("[INFO] cliente diz: %s\n", mensagem);
        }
        
    } while (strcmp(mensagem, "tchau"));

    /* fechando conexao */
    close(socket_cliente);
    close(socket_servidor);
    printf("[INFO] conexao fechada!\n");

    return EXIT_SUCCESS;
}
