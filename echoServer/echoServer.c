#include <asm-generic/socket.h>
#include <locale.h>
#include <unistd.h> // for close(sockfd);
#include <netdb.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/wait.h>

#define PORT "1337"

int main(int argc, char *argv[])
{
    struct addrinfo hints, *servinfo, *p;
    socklen_t sin_size;
    int getI;
    int sockfd, newsockfd;
    int optval;
    char echo_input[1024];

    //speicher frei machen und felder für addrinfo struct setzen
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    if((getI = getaddrinfo("127.0.0.1", PORT, &hints, &servinfo)) != 0){
        fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(getI));
        return 1;
    }
    
    p = servinfo;
    if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
        printf("Socket on %p konnte nicht erstellt werden", p);
        exit(2);
    }
    // SOL_SOCKET for protocol independent option
    if((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int))) == -1){
        perror("setsockopt");
        exit(1);
    }
    
    if((bind(sockfd, p->ai_addr, p->ai_addrlen)) == -1){
        close(sockfd);
        perror("echo server : bind");
    }
    
    // freeaddrinfo(servinfo); macht den Speicher frei, der von der Funktion getaddrinfo() für die Adressinformationen (der Netzwerkschnittstellen) zugewiesen wurde.    
    freeaddrinfo(servinfo);     
    if(p == NULL){
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }
    if((listen(sockfd, 1)) == -1){
        perror("listen");
        exit(1);
    }
    // does itself return -1 in an error
    // p wird benutzt da ich meine eigenes echo möchte
    newsockfd = accept(sockfd, p->ai_addr, &sin_size);
    
    while(1){
        
        if(!(recv(newsockfd, &echo_input, 1024, 0))){
            break;
        }
        printf("msg from %s -> %s", p->ai_addr->sa_data, echo_input);
        if((send(newsockfd, echo_input, strlen(echo_input), 0)) == -1){
            perror("send");
        }
    }
    close(newsockfd);
  return 0;
}
