#ifndef HEADER_MASTER
#define HEADER_MASTER

//TODO: passar para português

/* declaracao das constantes */
#define PORT 4242       /* definindo a porta de conexão  */
#define PCKT_LEN 1500   /* definicao do tamanho do pacote */
#define TAMANHO_MENSAGEM 100 /* definicao do tamanho da mensagem */

#define ETHERNET_ADDR_LEN 6
#define IPV6_ADDR_LEN 16
#define IP_ADDR_LEN 4
#define ARP_PADDING_SIZE 18
#define ETHERTYPE 0x86DD /** indicando que é do tipo IPv6 **/
#define ARPHRD_ETHER 1
#define ETH_P_IP 0x0800
#define ARPOP_REQUEST 1
#define ARPOP_REPLY 2

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
    unsigned char tcph_reserved : 4;
    unsigned char tcph_offset : 4;
    unsigned char tcph_flags;
    unsigned short int windows_size;
    unsigned short int checkSum;
    unsigned short int urg_pointer;
    /* Dados */
    char mensagem[TAMANHO_MENSAGEM];
} estrutura_pacote;

#endif