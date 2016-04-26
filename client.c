#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <resolv.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

#define MAXBUF 256
//#define PORT_FTP 8888
#define SERVER_ADDR "127.0.0.1"


void * handleServer(int sockfd) {
    
    char buffer[MAXBUF];
    bzero(buffer, 255);
    int n = 0;
    
    while (1) {
        
        n = recv(sockfd, buffer, MAXBUF, NULL);
        if (n == 0) {
            break;
        }
        printf("%s\n", buffer);
        bzero(buffer, 255);
        
    }
    
    printf("\nServer ended connection\n");
    
}

void * handleCommand(int sockfd) {
    
    printf("Enter a command: ");         
    char buffer[MAXBUF];
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
        printf("Processing command...\n");        
        sleep(2);                                               //slows down process to simulate a server handling thousands of requests
        printf("Enter a command: ");     
        
    }
    
}

int main (int argc, char **argv) {
    
    struct addrinfo ai;
    struct addrinfo *dest;
    
    ai.ai_flags = 0;
	ai.ai_family = AF_INET;
	ai.ai_socktype = SOCK_STREAM;
	ai.ai_protocol = 0;
    ai.ai_addrlen = 0;
    ai.ai_addr = NULL;
    ai.ai_canonname = NULL;
    ai.ai_next = NULL;
    bzero(&dest, sizeof(dest));
    
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    int ai_flag = getaddrinfo(argv[1], argv[2], &ai, &dest);
    
    /*Connect to Server*/
    int con = connect(sockfd, (struct sockaddr*)&dest, sizeof(dest));
    
    /*If first connection attempt failed, wait 3 seconds and try again*/
    while (con != 0) {
        sleep(3);
        con = connect(sockfd, dest->ai_addr, dest->ai_addrlen);
    }
    //freeaddrinfo(dest);
    printf("Successfully connected to server! Accepting commands now.\n");
    
    /*Create threads to handle user input and server response*/
    pthread_t commandInput;
    pthread_t output;
    pthread_create(&commandInput, NULL, handleCommand, sockfd);
    pthread_create(&output, NULL, handleServer, sockfd);
    
    
    //Wait for threads to finish
    pthread_join(output, NULL);
    pthread_cancel(output);
    
    close(sockfd);
    
    return 0;
}