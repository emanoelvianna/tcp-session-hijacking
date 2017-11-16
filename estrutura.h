#ifndef HEADER_MASTER
#define HEADER_MASTER

//TODO: passar para português

/* declaracao das constantes */
#define PORTA_SERVIDOR 4242 /* definindo a porta de conexão  */
#define PORTA_CLIENTE 4200  /* definindo a porta de conexão  */

#define PCKT_LEN 1500        /* definicao do tamanho do pacote */
#define TAMANHO_MENSAGEM 100 /* definicao do tamanho da mensagem */

#define ETHERNET_ADDR_LEN 6
#define IPV6_ADDR_LEN 16
#define ETHERTYPE 0x86DD /** indicando que é do tipo IPv6 **/

/* definições de endereço IP */
#define IP_SERVIDOR "::1"
#define IP_CLIENTE "::2"

/* definições de endereço MAC */
#define MAC_SERVIDOR "78:2b:cb:ed:c4:37"
#define MAC_CLIENTE "78:2b:cb:ed:c4:37"

typedef struct
{
    /* Cabeçalho Ethernet */
    unsigned char target_ethernet_address[ETHERNET_ADDR_LEN]; // endereco_fisico_destino
    unsigned char source_ethernet_address[ETHERNET_ADDR_LEN]; // endereco_fisico_origem
    unsigned short ethernet_type;
    /* Pacote IPv6 */
    unsigned char version : 6;
    unsigned char traffic_class;
    unsigned int flow_label;
    unsigned short payload_length;
    unsigned char next_header;
    unsigned char hop_limit;
    unsigned char source_address[IPV6_ADDR_LEN];
    unsigned char destination_address[IPV6_ADDR_LEN];
    // extension_header;
    /* Pacote TCP */
    unsigned short int source_port;
    unsigned short int destination_port;
    unsigned int sequence_number;
    unsigned int ack_number;
    unsigned char tcph_reserved;
    unsigned char tcph_offset;
    unsigned char tcph_flags;
    unsigned short int windows_size;
    unsigned short int checkSum;
    unsigned short int urg_pointer;
    /* Dados */
    char mensagem[TAMANHO_MENSAGEM];
} estrutura_pacote;

#endif