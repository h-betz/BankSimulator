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

//Creates an account with name acct_name and returns the account struct
Account * createAccount(char *acct_name) {
    
    Account *acct = malloc(sizeof(Account));
    if (acct == NULL) {
        printf("ERROR! Failed to create account %s.\n", acct_name);
        return NULL;
    }
    
    bzero(acct->name, 100);
    strcpy(acct->name, acct_name);
    acct->balance = 0;
    acct->in_session = 0;
    return acct;
    
}

//Add the amount to the specified account
void creditAccount(float amount, Account *acct) {
    
    acct->balance += amount;
    
}

//Debit the account. Return zero if an overdraw is attempted
int debitAccount(float amount, Account *acct) {
    
    if (amount > acct->balance) {
        return 0;
    } else {
        acct->balance -= amount;
        return 1;
    }
    
}

//Return the account balance
float getBalance(Account *acct) {
    
    return acct->balance;
    
}