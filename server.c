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
#include "bank.h"
#include "clientHandle.h"

#define PORT 8888
#define MAXBUF 1024

//Global variables for threads, accounts, and server control
pthread_t *tid;
Account accounts[20];
int terminate = 0;
int sockfd;


//Keeps our server running for 5000 seconds before shutting down
void * timer() {
    
    sleep(5000);
    terminate = 1;
    terminateThreads(tid);
    
}

//Ends all connections and threads in our server
void terminateThreads(pthread_t *threads) {
    
    int i = 0;
    for (i = 0; i < 20; i++) {
        pthread_cancel(threads[i]);
    }
    
}

//Simulates a server backup that occurs every 20 seconds
void serverControl() {
    
    while (1) {
        sleep(20);
    }
    
}

//Generic function to handle client-server interaction
void * get_result(int clientfd) {
    
    int compare = -1;    
    char buffer[MAXBUF];
    bzero(buffer, MAXBUF);
    char *result;
    float amount = 0;
    int flag = -1;
    int n = read(clientfd, buffer, 255);
    compare = strcmp("exit\n", buffer);
        
    //As long as user doesn't exit, continue to received messages from user
    while (compare != 0 && n != 0) {
        flag = check(buffer);
        switch (flag) {
            case 1:
                result = readAccountName(buffer, strlen("open "));
                free(result);        
                break;
            case 2:
                result = readAccountName(buffer, strlen("start "));
                free(result);
                break;
            case 3:
                amount = readCreditDebit(buffer, strlen("debit "));
                break;
            case 4:
                amount = readCreditDebit(buffer, strlen("credit "));
                break;
        }
        /*result = tokenize(buffer);
        printf("%s\n", result);*/
        bzero(buffer, MAXBUF);
        n = read(clientfd, buffer, 255);
        compare = strcmp("exit\n", buffer);

    }
    
    //Close socket with client and display status update
    printf("Connection with %d terminated.\n", clientfd);
    close(clientfd);
}


int main(int argc, char **argv) {
    
    struct sockaddr_in self;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
    if (sockfd < 0) {
        perror("Socket");
        exit(errno);
    }
    
    /*Initialize address and port structure */
    bzero(&self, sizeof(self));
    self.sin_family = AF_INET;
    self.sin_port = htons(PORT);
    self.sin_addr.s_addr = INADDR_ANY;
    
    /*Assign port number to socket*/
    if (bind(sockfd, (struct sockaddr*)&self, sizeof(self)) != 0) {
        perror("Socket -- bind");
        exit(errno);
    }
    
    /*Make it a listening socket*/
    if (listen(sockfd, 20) != 0) {
        perror("Socket -- listen");
        exit(errno);
    }
    
    int clientfd;
    int id = 0;
    tid = malloc(20 * sizeof(pthread_t));
    pthread_t *timeThread;                                                      //our thread that will control how long our server is online
    pthread_t *control;                                                         //our thread that will handle requests from server manager
    
    //Start the server
    while (!terminate) {
        
        pthread_create(&control, NULL, serverControl, NULL);
        pthread_create(&timeThread, NULL, timer, NULL);        
        struct sockaddr_in client_addr;
        int addrlen = sizeof(client_addr);
        printf("Listening...\n");
        
        /*accept a connection and spawn a new thread*/
        clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);
		printf("%s:%d connected\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));        
        pthread_create(&tid[id], NULL, get_result, clientfd);
        
    }
    
    //Close all sockets and connections   
    free(tid); 
    close(clientfd);
    close(sockfd);
    
    return 0;
}