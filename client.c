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

/*char * readAccountName(char *str, int i) {
    
    char *acct_name = (char *)malloc(strlen(str) + 1);
    bzero(acct_name, strlen(str) + 1);
    int ind = 0;
    char c = str[i];
    if (isalnum(c)) {
        
        while (isalnum(c)) {
            acct_name[ind] = c;
            ind++;
            c = str[++i];
        }
        
        if (c == '\0') {
            return acct_name;
        }
        
    }
    
    return 0;
}

float readCreditDebit(char *str, int i) {
    
    char *amount = (char *) malloc(strlen(str) + 1);
    bzero(amount, strlen(str) + 1);
    int ind = 0;
    char c = str[i];
    
    if (isdigit(c)) {
        
        while (isdigit(c)) {
            amount[ind] = c;
            ind++;
            c = str[++i];
        }
        
        if (c == '.') {
            amount[ind] = c;
            ind++;
            c = str[++i];
            while (isdigit(c)) {
                amount[ind] = c;
                ind++;
                c = str[++i];
            }
            if (c == '\0') {
                //return as float
            } else {
                return -1;
            }
        } else {
            return -1;
        }
        
    } else if (c == '.') {
        
    }
    
    return -2;
}

int checkCommand(char *token, char *str, int i) {
    
    if (strcmp("open", token) == 0) {
        //Command is open
        char *acct_name = readAccountName(str, i);
        return 0;
    } else if (strcmp("start", token)) {
        //Command is start
        char *acct_name = readAccountName(str, i);        
        return 1;
    } else if (strcmp("debit", token)) {
        //Command is debit
        float debit = readCreditDebit(str, i);
        return 2;
    } else if (strcmp("credit", token)) {
        //Command is credit
        float credit = readCreditDebit(str, i);
        return 3;
    }

    return -1;
}

//Parses user input to read commands
void tokenize(char *str) {
    
    char *token = (char *) malloc(strlen(str));
    bzero(token, strlen(str));
    char c = str[0];
    int i = 0;
    int result = -2;
    
    //Move through each character of the string to read it
    if (isalpha(c)) {
        token[i] = c;
        c = str[i];
        while (isalpha(c)) {
            token[++i] = 
            c = str[i];
        }
        //If next character is a space, marks end of command (eg. debit, open, credit)
        if (c == ' ') {
            result = checkCommand(token, str, ++i);
            free(token);
            switch(result) {
                case 0:
                    //read account name
                    break;
                case 1:
                    //start account
                    break;
                case 2:
                    //readCreditDebit
                    break;
                case 3:
                    //readCreditDebit
                    break;
                default:
                
            }
            //switch case to handle command
        } else if (c == '\0') {
            if (strcmp("balance", token)) {
                //Command is balance
            } else if (strcmp("finish", token)) {
                //Command is finish
            }
        } else {
            //Print error -- Not a viable command
        }
        
    }
    
}*/

void * handleServer() {
    
    
    
}

void * handleCommand(int sockfd) {
    
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
        sleep(2);                                               //slows down process to simulate a server handling thousands of requests
        
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
    pthread_create(&output, NULL, handleServer, NULL);
    
    
    //Wait for threads to finish
    pthread_join(commandInput, NULL);
    pthread_cancel(output);  
    
    close(sockfd);
    
    return 0;
}