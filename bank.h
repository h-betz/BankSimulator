#ifndef BANK_H
#define BANK_H

typedef struct Acount_ {
    
    char name[100];
    float balance;
    int in_session;
    pthread_mutex_t mutex;

} Account;

Account * createAccount(char *name);

void creditAccount(float amount, Account *acct);

int debitAccount(float amount, Account *acct);

float getBalance(Account *acct);

#endif
