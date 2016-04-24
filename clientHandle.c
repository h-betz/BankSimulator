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
#include "clientHandle.h"
#include "bank.h"



char * readAccountName(char *str, int i) {
    
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

        if (isspace(c)) {
            acct_name[ind] = '\0';
            return acct_name;
        }
        
    }
    
    return 0;
}

//Gets the amount to credit or debit to an account and returns it as a floating point value
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
            if (c == '\0' || isspace(c)) {
                float f = atof(amount);
                free(amount);
                return f;
            } else {
                return 0;
            }
        } else {
            return 0;
        }
        
    } else if (c == '.') {
        amount[ind] = c;
        ind++;
        c = str[++i];
        if (!isdigit(c)) {
            return 0;
        }
        while (isdigit(c)) {
            amount[ind] = c;
            ind++;
            c = str[++i];
        }
        if (c == '\0') {
            float f = atof(amount);
            free(amount);
            return f;
        }
    }
    
    return 0;
}

//Checks what command was given to the server
int check(char *string) {
    
    char *result = tokenize(string);
    if (strcmp(result, "open") == 0) {
        //Command is open
        free(result);
        return 1;
    } else if (strcmp(result, "start") == 0) {
        //Command is start
        free(result);        
        return 2;
    } else if (strcmp(result, "debit") == 0) {
        //Command is debit
        free(result);       
        return 3;
    } else if (strcmp(result, "credit") == 0) {
        //Command is credit
        free(result);
        return 4;
    } else if (strcmp(result, "finish") == 0) {
        free(result);
        return 5;
    } else if (strcmp(result, "balance") == 0) {
        free(result);
        return 6;
    }
    
    return 0;
}


//Parses user input to read commands
char * tokenize(char *str) {
    
    char *token = (char *) malloc(strlen(str));
    memset(token, '\0', strlen(str));
    char c = str[0];
    int i = 0;
    
    //Move through each character of the string to read it
    if (isalpha(c)) {
        token[i] = c;
        c = str[++i];
        while (isalpha(c)) {
            token[i] = c;
            c = str[++i];
        }
        //If next character is a space, marks end of command (eg. debit, open, credit)
        if (c == ' ') {
            return token;
            //switch case to handle command
        } else if (c == '\0' || isspace(c)) {
            if (strcmp("balance", token)) {
                //Command is balance
                return token;
            } else if (strcmp("finish", token)) {
                //Command is finish
                return token;
            }
        } else {
            //Print error -- Not a viable command
        }
        
    }
    
    return 0;
    
}
