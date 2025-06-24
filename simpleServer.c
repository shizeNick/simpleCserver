/*
 * simpleServer.c -- stream socket server demo
 */

#include <asm-generic/socket.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "3333" // port users will connect to

#define BACKLOG 10 // how many pending connections quere will hold

void sigchld_handler(int s)
{
    (void)s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6
void *get_in_addr(struct sockaddr * sad)
{
    if(sad->sa_family == AF_INET){
        return &(((struct sockaddr_in *)sad)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sad)->sin6_addr);
}

int main(void)
{
    int sockfd, new_fd; // listen on sock_fd, new connection on new fd
    struct addrinfo hints, *servinfo, *p; // - hints ist der name der varibale vom typ struct addrinfo
                                          // - *servinfo ist ein pointer auf eine addrinfo, also auf eine von n verbindungen. 
                                          //   Diese entsprechen dann den festgelegte Hints kriterien
                                          //    
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s [INET6_ADDRSTRLEN];
    int rv;

    // felder von hints werden ausgefüllt, die getaddrinfo() verwenden soll
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP , für Clients wird es nicht gebraucht
   
    // getaddrinfo() weist den Speicher für die Ergebnisliste (servinfo) automatisch zu, also die Information die für eine verbindung, gebraucht werden
    // &hints ist die Adresse der hints-Struktur. Diese Struktur dient als Eingabeparameter, um getaddrinfo() mitzuteilen, welche Kriterien die gewünschten Adressinformationen erfüllen sollen 
    // NULL wird meisten bei einem server gesetzt, da dieser sagt, dass ich auf alle vorhandenen Netzwerkschnittstellen des servers lauchen möchte 
    //   -> man könnte aber auch nur eine spezifische lokale ip addresse angeben wie die loopback vom server, wlan0 vom server oder die eth.
    // Und auf der &servinfo addresse werden schlussendlich die erfassten potenziellen verbindungen als eine Liste gespeichert
    if((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // durchläuft die erfasste Liste auf &servinfo um jeweils das Socket auf das Interface aufzusetzn
    for(p = servinfo; p != NULL; p = p -> ai_next){
        if ((sockfd = socket(p -> ai_family, p -> ai_socktype, p -> ai_protocol)) == -1){
            perror("server : bind");
            printf("Socket auf %p konnte nicht erstellt werden!", *p);
            continue;
        }

        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
            perror("setsockopt");
            exit (1);
        }

        if(bind(sockfd, p -> ai_addr, p -> ai_addrlen) == -1){
            close(sockfd);
            perror("server : bind");
            continue;
        }

        break;
        
    }

    // freeaddrinfo(servinfo); macht den Speicher frei, der von der Funktion getaddrinfo() für die Adressinformationen (der Netzwerkschnittstellen) zugewiesen wurde.    
    freeaddrinfo(servinfo);     if(p == NULL){
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }
 
    // listen(sockfd, BACKLOG): Versetzt den Socket in den passiven Modus,
    // um auf eingehende Verbindungsanfragen zu lauschen und diese in einer
    // Warteschlange (mit maximal BACKLOG ausstehenden Verbindungen) zu halten,
    // bis sie mit accept() verbunden werden
    // -->  accpet() ist dann der eigentliche erste Kontakt mit dem Client
    if(listen(sockfd, BACKLOG) == -1){
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);

    sa.sa_flags = SA_RESTART;
    if(sigaction(SIGCHLD, &sa, NULL) == -1){
        perror("sigaction");
        exit(1);
    }

    printf("server : waiting for connections...\n");

    while (1) { // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if(new_fd == -1){
            perror("accept");
            continue;
        }
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("server : got connection from %s\n", s);

        if(!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener
            char welcome[] = "Sie sind mit dem Server verbunden\n"
                             "Commands:\n"
                             " [q] : Disconnect\n"
                             " [g] : Game-Menu\n";
            if(send(new_fd, welcome, strlen(welcome), 0) == -1)
                perror("send");
            char client_input;
            // quit = getchar(); --> recv() verwenden, da man von einem socket liest
            ssize_t received = recv(new_fd, &client_input, 1, 0); // versucht 1 Byte zu empfangen
            // ssize_t -> ss(signed size) wird für die anzahl von bytes oder die Größe von Daten verwendet
            //            die von Funktionen zurückgegeben werden kann (kompatibler als int)
            switch(client_input){
                case 'q':
                    printf("%s disconnected", s);
                    if(send(new_fd, "good Bye :)\n", 11, 0))
                        perror("send");
                    close(new_fd);
                case 'g':
            }
            exit(0);
        }
        close(new_fd);  // parent doesnt't need this
    }
    return 0;
}


// inet_ntop converts an IP address in numbers-and-dots notaion into either a struct in_addr or struct in6_add
// pton -> presentation to network
