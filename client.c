#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <resolv.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

#define MAXBUF 1024
#define PORT_FTP 8888
#define SERVER_ADDR "127.0.0.1"

int main (int argc, char **argv) {
    
    int sockfd = 0;
    struct sockaddr_in dest;
    char buffer[MAXBUF];
    
    /*Open socket for streaming*/
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd < 0) {
        perror("Socket");
        exit(errno);
    }
    
    /*Initialize server address/port struct*/
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(PORT_FTP);
    
    if (inet_aton(SERVER_ADDR, &dest.sin_addr.s_addr) == 0) {
        perror(SERVER_ADDR);
        exit(errno);
    }
    
    /*Connect to Server*/
    int con = connect(sockfd, (struct sockaddr*)&dest, sizeof(dest));
    
    /*If first connection attempt failed, wait 3 seconds and try again*/
    while (con != 0) {
        sleep(3);
        con = connect(sockfd, (struct sockaddr*)&dest, sizeof(dest));
    }
    /*if (con != 0) {
        perror("Connect");
        exit(errno);
    }*/
    printf("Successfully connected to server! Accepting commands now.\n");
    int comp = -1;
    int length = 0;
    
    //Get user input and send it to server
    while (1) {
        bzero(buffer, MAXBUF);        
        if (fgets(buffer, MAXBUF, stdin) == NULL) break;
        comp = strcmp("exit\n", buffer);
        length = strlen(buffer);
        send(sockfd, buffer, length, 0); 
        if (comp == 0) {
            break;
        }
        sleep(2);                                               //slows down process to simulate a server handling thousands of requests
        
    }
       
    
    close(sockfd);
    
    return 0;
}