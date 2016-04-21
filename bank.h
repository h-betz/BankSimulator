#ifndef BANK_H
#define BANK_H

typedef struct Acount_ {
    
    char name[100];
    float balance;
    int in_session;

} Account;

Account * createAccount(char *name);

#endif
