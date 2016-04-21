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

Account * createAccount(char *acct_name) {
    
    Account *acct = malloc(sizeof(Account));
    if (acct == NULL) {
        printf("ERROR! Failed to create account %s.\n", acct_name);
        return NULL;
    }
    
    bzero(acct->name, 100);
    strcpy(acct->name, acct_name);
    acct->balance = 0;
    acct->in_session = 1;
    return acct;
    
}