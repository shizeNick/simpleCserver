#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <unistd.h>

void print_mac(const unsigned char *mac){
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

// Format: "AA:BB:CC:DD:EE:FF"
int mac_aton(const char *mac_str, unsigned char *mac_bytes) {
    unsigned int values[6];
    int i;

    // Versucht, 6 Hex-Werte zu lesen, getrennt durch Doppelpunkte
    if (sscanf(mac_str, "%x:%x:%x:%x:%x:%x",
               &values[0], &values[1], &values[2],
               &values[3], &values[4], &values[5]) != 6) {
        return -1; // Fehler beim Parsen
    }

    for (i = 0; i < 6; i++) {
        mac_bytes[i] = (unsigned char) values[i];
    }
    return 0; // Erfolg
}

// Eine Struktur, die den gesamten ARP-Frame darstellt (Ethernet + ARP-Header + Adressen)
// Dies ist eine gängige Methode, um das Paket zu modellieren.
// Die Größen sind fest für Ethernet/IPv4 ARP.
struct arp_packet {
    struct ethhdr eth_header; // 14 bytes
    struct arphdr arp_header; // 8 bytes (generischer ARP-Header)
    unsigned char sender_mac[ETH_ALEN]; // 6 bytes
    struct in_addr sender_ip;           // 4 bytes
    unsigned char target_mac[ETH_ALEN]; // 6 bytes
    struct in_addr target_ip;           // 4 bytes
};
// Gesamtgröße: 14 + 8 + 6 + 4 + 6 + 4 = 42 bytes

int main(int argc, char *argv[])
{
    if(argc < 3 || argc > 4){
        printf("%s + <target> <spoof_ip> [target-mac]", argv[0]);
        exit(2);
    }
    const char* iface_name = "wlan0";
    const char* target_ip = argv[1];
    const char* spoof_ip = argv[2];
    
    int sockfd; // Unser Raw Socket File Descriptor
    int ioctl_sockfd;
    struct ifreq if_idx; // Für Interface-Index
    struct ifreq if_mac; // Für MAC-Adresse

    unsigned char local_mac[6]; // Lokale MAC-Adresse
    unsigned char* target_mac;
    unsigned char broadcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    if(argc == 3){
        mac_aton(argv[3], target_mac);
    }else{
        target_mac = broadcast_mac; // Standard = Broadcast MAC-Adresse
    }
    int if_index; // Index des Interfaces


    if((ioctl_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        perror("socket");
        return 1;
    }

    printf("Raw-Socket erstellt.");

    if((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) == -1){
        perror("socket");
        return 1;
    }
    printf("Raw-Socket erstellt.");
    
    // Interface Index ermitteln
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_ifrn.ifrn_name, iface_name, IFNAMSIZ -1);
    if((ioctl(ioctl_sockfd, SIOCGIFINDEX, &if_idx)) == -1){
        perror("SIOCGIFINDEX");
        close(ioctl_sockfd);
        close(sockfd);
        return 1;
    }
    if_index = if_idx.ifr_ifru.ifru_ivalue;
    printf("Interface: %s\nIndex: %d\n", iface_name, if_index);

    // Eigende Mac ermitteln
    memset(&if_mac, 0, sizeof(struct ifreq));
    strncpy(if_mac.ifr_ifrn.ifrn_name, iface_name, IFNAMSIZ -1);
    if(ioctl(ioctl_sockfd, SIOCGIFHWADDR, &if_mac) == -1){
        perror("SIOCGIFHWADDR");
        close(ioctl_sockfd);
        close(sockfd);
        return 1;
    }
    memcpy(local_mac, if_mac.ifr_ifru.ifru_hwaddr.sa_data, 6);
    printf("Eigene MAC-Adresse: ");
    print_mac(local_mac);

    // create ETH-Header Frame
    struct arp_packet *eth_h;
    memcpy(eth_h->eth_header.h_dest, target_mac, ETH_ALEN);
    memcpy(eth_h->eth_header.h_source, local_mac, ETH_ALEN);
    eth_h->eth_header.h_proto = htons(ETH_P_ARP); // 0x0806
                                                  //
    printf("Ethernet-Header erstellt.\n");

    // create ARP-Header Frame
    struct arp_packet *arp_p; 
    memcpy(arp_p->sender_mac, local_mac, ETH_ALEN);
    arp_p->sender_ip = inet_addr(spoof_ip);

    memcpy(arp_p->target_mac, , size_t n)
    close(sockfd);
    
    return 0;
}


