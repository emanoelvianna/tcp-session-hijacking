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

int main(void) {

    /* Client and Server socket structures */
    struct sockaddr_in6 client, server;

    /* File descriptors of client and server */
    int serverfd, clientfd;

    char buffer[TAMANHO_MENSAGEM];

    fprintf(stdout, "Starting server\n");

    /* Creates a IPv4 socket */
    serverfd = socket(AF_INET6, SOCK_STREAM, 0);
    if(serverfd == -1) {
        perror("Can't create the server socket:");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Server socket created with fd: %d\n", serverfd);


    /* Defines the server socket properties */
    server.sin6_family = AF_INET6;
    server.sin6_port = htons(PORTA_SERVIDOR);
	inet_pton(AF_INET6, "::1", &server.sin6_addr);

    /* Handle the error of the port already in use */
    int yes = 1;
    if(setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR,
                  &yes, sizeof(int)) == -1) {
        perror("Socket options error:");
        return EXIT_FAILURE;
    }


    /* bind the socket to a port */
    if(bind(serverfd, (struct sockaddr*)&server, sizeof(server)) == -1 ) {
        perror("Socket bind error:");
        return EXIT_FAILURE;
    }


    /* Starts to wait connections from clients */
    if(listen(serverfd, 1) == -1) {
        perror("Listen error:");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Listening on port %d\n", PORTA_SERVIDOR);

    socklen_t client_len = sizeof(client);
    if ((clientfd=accept(serverfd,
        (struct sockaddr *) &client, &client_len )) == -1) {
        perror("Accept error:");
        return EXIT_FAILURE;
    }

    /* Copies into buffer our welcome messaage */
    strcpy(buffer, "Hello, client!\n\0");

    /* Sends the message to the client */
    if (send(clientfd, buffer, strlen(buffer), 0)) {
        fprintf(stdout, "Client connected.\nWaiting for client message ...\n");

        /* Communicates with the client until bye message come */
        do {

            /* Zeroing buffers */
            memset(buffer, 0x0, TAMANHO_MENSAGEM);

            /* Receives client message */
            int message_len;
            if((message_len = recv(clientfd, buffer, TAMANHO_MENSAGEM, 0)) > 0) {
                buffer[message_len - 1] = '\0';
                printf("Client says: %s\n", buffer);
            }

            /* 'bye' message finishes the connection */
            if(strcmp(buffer, "bye") == 0) {
                send(clientfd, "bye", 3, 0);
            } else {
                send(clientfd, "yep\n", 4, 0);
            }

        } while(strcmp(buffer, "bye"));
    }

    /* Client connection Close */
    close(clientfd);
    /* Close the local socket */
    close(serverfd);
    printf("Connection closed\n\n");

    return EXIT_SUCCESS;
}
