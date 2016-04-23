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
#include <signal.h>
#include "bank.h"
#include "clientHandle.h"

#define PORT 8888
#define MAXBUF 256

//Global variables for threads, accounts, and server control
pthread_t *tid;
pthread_mutex_t mutex;
Account *accounts[20];
int terminate = 0;
int sockfd;

//Handles system signlas such as ^C and shutsdown the server in a graceful, controlled manner
void intHandler(int sig) {
    
    terminate = 1;
    bernieSanders();
    terminateThreads();
    free(tid);
    close(sockfd);
    printf("Terminated\n");
    exit(errno);
}


//Keeps our server running for 5000 seconds before shutting down
void * timer() {
    
    sleep(5000);
    terminate = 1;
    terminateThreads();
    
}

//Ends all connections and threads in our server
void terminateThreads() {
    
    int i = 0;
    for (i = 0; i < 20; i++) {
        if (tid[i] != NULL) {
            pthread_cancel(tid[i]);
        }
    }
    
}

//Simulates a server backup that occurs every 20 seconds
void serverControl() {
    
    while (1) {
        sleep(20);
    }
    
}

//Closes all accounts in our bank
void bernieSanders() {
    
    int i = 0;
    Account *acct = accounts[i];
    while (acct != NULL) {
        printf("Breaking up the big banks!\n");
        free(acct);
        ++i;
        acct = accounts[i];
    }
    
}

//Adds the account with name acct to our list of accounts 
void addAccount(Account *acct) {
    
    int i = 0;
    //Loops through our list of accounts to see if there is an opening or if 
    //an account with the same name already exists
    for (i = 0; i < 20; i++) {
        if (accounts[i] == NULL) {
            accounts[i] = acct;
            return;
        }
        if (strcmp(accounts[i]->name, acct->name) == 0) {
            printf("An account with this name already exists!\n");
            return;
        }
    }
    
}

Account * getAccount(char *name) {
    
    int i = 0;
    
    for (i = 0; i < 20; i++) {
        //Check if this is our account
        if (strcmp(accounts[i]->name, name) == 0) {
            //Account is in session
            if (accounts[i]->in_session != 0) {
                printf("Account is in session.\n");
                return NULL;
            }
            return accounts[i];
        }
    }
    
    return NULL;
    
}

//Handles a customer session
void customerSession(Account *acct, int clientfd) {
    
    int compare = -1;                               //compare integer to check when user says finish
    char buffer[MAXBUF];                            //char array to store user command
    bzero(buffer, MAXBUF);                          //Zero out char array
    int flag = -1;                                  //Flag to store result of command
    float amount = 0;                               //Floating value to store value to debit, credit or balance
    int n = read(clientfd, buffer, 255);            //Read commands from client
    
    compare = strcmp("finish\n", buffer);           //Customer ended this session
    
    while (compare != 0 && n != 0) {
        printf("Customer session\n");
        flag = check(buffer);
        switch (flag) {
            case 3:
                amount = readCreditDebit(buffer, strlen("debit "));
                break;
            case 4:
                amount = readCreditDebit(buffer, strlen("credit "));
                break;
            case 5:
                //finish account session
                compare = 1;
                break;
            case 6:
                //get balance
                break; 
        }
        
        bzero(buffer, MAXBUF);
        n = read(clientfd, buffer, 255);
        
    }
    
}

//Generic function to handle client-server interaction
void * get_result(int clientfd) {
    
    int compare = -1;                           //Compare value to compare commands with exit strin
    char buffer[MAXBUF];                        //Buffer array to store client input
    bzero(buffer, MAXBUF);                      //Zeros out our buffer stream
    char *result;                               //String to hold account name
    float amount = 0;                           //Floating value to store credit, debit, and balance values
    int flag = -1;                              //integer value to tell our switch case to perform which command
    Account *acct = NULL;                       //Account struct that will be used when opening or starting accounts
    int n = read(clientfd, buffer, 255);        //Integer to handle read result 
    compare = strcmp("exit\n", buffer);         //Checks to see if we were told to exit
        
    //As long as user doesn't exit, continue to received messages from user
    while (compare != 0 && n != 0) {
        flag = check(buffer);
        switch (flag) {
            case 1:
                result = readAccountName(buffer, strlen("open "));
                acct = createAccount(result);
                free(result);
                if (acct != NULL) {
                    pthread_mutex_lock(&mutex); 
                    addAccount(acct);
                    pthread_mutex_unlock(&mutex);
                }       
                break;
            case 2:
                result = readAccountName(buffer, strlen("start "));
                acct = getAccount(result);
                if (acct != NULL) {
                    acct->in_session = 1;
                    customerSession(acct, clientfd);
                }
                free(result);
                break;
            default:
                printf("Error! Improper command!\n");
                break;
        }

        bzero(buffer, MAXBUF);
        n = read(clientfd, buffer, 255);
        compare = strcmp("exit\n", buffer);

    }
    
    //Close socket with client and display status update
    printf("Connection with %d terminated.\n", clientfd);
    close(clientfd);
}


int main(int argc, char **argv) {
        
    int i = 0;
    for (i = 0; i < 20; i++) { 
        accounts[i] = NULL;
    }    
    
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
    
    struct sigaction act;
    act.sa_handler = intHandler;
    sigaction(SIGINT, &act, NULL);
    int clientfd;
    int id = 0;
    tid = malloc(20 * sizeof(pthread_t));
    pthread_t *timeThread;                                                      //our thread that will control how long our server is online
    pthread_t *control;                                                         //our thread that will handle requests from server manager
    pthread_mutex_init(&mutex, NULL);
    
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
    
    close(clientfd);
    
    return 0;
}