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

//Handles the commands entered by the user
void * handleCommand(int sockfd) {
    
    printf("Enter a command: ");         
    char buffer[MAXBUF];
    int comp = -1;
    int length = 0;
    
    //Get user input and send it to server
    while (1) {
        
        bzero(buffer, MAXBUF);                                      //zero out our char array that stores user input 
        if (fgets(buffer, MAXBUF, stdin) == NULL) {                 //get user input, if user enters nothing, do nothing
        } else {      
            if (strcmp(buffer, "") == 0) {
            } else {                                             
                comp = strcmp("exit\n", buffer);                        //checks to see if the user wants to exit
                length = strlen(buffer);
                send(sockfd, buffer, length, 0);                        //otherwise send user command to server
            }
        }
        
        //If user wants to exit, break from the loop
        if (comp == 0) {
            break;
        }
        
        printf("Processing command...\n");        
        sleep(2);                                               //slows down process to simulate a server handling thousands of requests
        printf("Enter a command: ");     
        
    }
    
}

//Does the leg work of connecting to the server found at the provided host name and port number
int main (int argc, char **argv) {
    
    char buffer[MAXBUF];
    //Checks to see if user entered valid arguments
    if (argc != 3) {
        printf("Please enter a valid host name and port number.\n");
        return 0;
    }
    
    int sockfd = 0;
    struct sockaddr_in dest;
    struct hostent *he;
    struct in_addr **addr_list;
    
    /*Open socket for streaming*/
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd < 0) {
        perror("Socket");
        exit(errno);
    }
    
    if ( (he = gethostbyname(argv[1]) ) == NULL ) {
        exit(1); /* error */
    }
    
    if (he == NULL) {
        exit(1);
    }
    
    /*Initialize server address/port struct*/
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    int p = atoi(argv[2]);
    dest.sin_port = htons(p);
    addr_list = (struct in_addr **) he->h_addr_list;
    const char *cp = inet_ntoa(*addr_list[0]);
    dest.sin_addr.s_addr = inet_addr(cp);
    
    /*Connect to Server*/
    printf("Connecting to server...\n");
    int con = connect(sockfd, (struct sockaddr*)&dest, sizeof(dest));
    
    /*If first connection attempt failed, wait 3 seconds and try again*/
    while (con != 0) {
        sleep(3);
        con = connect(sockfd, (struct sockaddr*)&dest, sizeof(dest));
    }
    
    printf("Success!\n");
    /*Create threads to handle user input and server response*/
    pthread_t commandInput;
    pthread_t output;
    pthread_create(&commandInput, NULL, handleCommand, sockfd);
    pthread_create(&output, NULL, handleServer, sockfd);
    
    
    //Wait for threads to finish
    pthread_join(output, NULL);
    pthread_cancel(commandInput); 
    
    close(sockfd);
    
    return 0;
}