#include <linux/if_ether.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <libnet.h>

void print_mac(const unsigned char *mac){
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

int main(int argc, char *argv[])
{
    if(argc < 3){
        printf("%s + <target> <spoof_ip>", argv[0]);
        exit(2);
    }
    const char* iface_name = "wlan0";
    const char* target = argv[1];
    const char* spoof_ip = argv[2];
    
    int sockfd; // Unser Raw Socket File Descriptor
    int ioctl_sockfd;
    struct ifreq if_idx; // Für Interface-Index
    struct ifreq if_mac; // Für MAC-Adresse
    unsigned char local_mac[6]; // Lokale MAC-Adresse
    int if_index; // Index des Interfaces

    if((ioctl_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
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
    if((ioctl(sockfd, SIOCGIFINDEX, SIOCGIFHWADDR)) == -1){
        perror("SIOCGIFINDEX");
        close(sockfd);
        return 1;
    }
    if_index = if_idx.ifr_ifru.ifru_ivalue;
    printf("Interface: %s\nIndex: %d\n", iface_name, if_index);

    // Eigende Mac ermitteln
    memset(&if_mac, 0, sizeof(struct ifreq));
    strncpy(if_mac.ifr_ifrn.ifrn_name, iface_name, IFNAMSIZ -1);
    if(ioctl(ioctl_sockfd, SIOCGIFHWADDR, &local_mac) == -1){
        perror("SIOCGIFHWADDR");
        close(ioctl_sockfd);
        return 1;
    }
    memcpy(local_mac, if_mac.ifr_ifru.ifru_hwaddr.sa_data, 6);
    printf("Eigene MAC-Adresse: ");
    print_mac(local_mac);

    close(sockfd);
    
    return 0;
}


