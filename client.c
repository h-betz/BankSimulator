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

#define MAXBUF 256
#define PORT_FTP 8888
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
    
    int sockfd = 0;
    struct sockaddr_in dest;
    
    
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

    printf("Successfully connected to server! Accepting commands now.\n");
    
    /*Create threads to handle user input and server response*/
    pthread_t commandInput;
    pthread_t output;
    pthread_create(&commandInput, NULL, handleCommand, sockfd);
    pthread_create(&output, NULL, handleServer, sockfd);
    
    
    //Wait for threads to finish
    //pthread_join(commandInput, NULL);
    //pthread_cancel(output);  
    pthread_join(output, NULL);
    pthread_cancel(output);
    
    close(sockfd);
    
    return 0;
}