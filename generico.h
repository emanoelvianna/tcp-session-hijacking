/* declaracao das constantes */
#define ETHERNET_ADDR_LEN 6
#define IPV6_ADDR_LEN 16
#define IP_ADDR_LEN 4
#define ARP_PADDING_SIZE 18
#define ETHERTYPE 0x86DD /** indicando que é do tipo IPv6 **/
#define ARPHRD_ETHER 1
#define ETH_P_IP 0x0800
#define ARPOP_REQUEST 1
#define ARPOP_REPLY 2

typedef enum {

    IPV6_TCP_HEADER = 6,
    IPV6_UDP_HEADER = 17,
    IPV6_ICMPV6_HEADER = 58

} Ipv6NextHeaderType;

typedef struct
{
    /* Cabeçalho Ethernet */
    unsigned char target_ethernet_address[ETHERNET_ADDR_LEN]; // endereco_fisico_destino
    unsigned char source_ethernet_address[ETHERNET_ADDR_LEN]; // endereco_fisico_origem
    unsigned short ethernet_type;
    /* Pacote IPv6 */
    unsigned char version : 4;                            // 4 bits
    unsigned char traffic_class;                          // 8 bits
    unsigned int flow_label;                              // 20 bits
    unsigned short payload_length;                        // 16 bits
    unsigned char next_header;                            // 8 bits
    unsigned char hop_limit;                              // 8 bits
    unsigned char source_address[IPV6_ADDR_LEN];      // 128 bits
    unsigned char destination_address[IPV6_ADDR_LEN]; // 128 bits
    // extension_header;
    /* Pacote TCP */
    
} estrutura_pacote;
