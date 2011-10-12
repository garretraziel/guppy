#include <stdio.h>

#include "lexical.h"


const char *TOKENS[] = {
    [NOTHING] = "NULL",
    [NUMBER] = "NUMBER",
    [OPERATOR] = "OPERATOR",
    [STRING] = "STRING",
    [IDENTIFIER] = "IDENTIFIER\0",
    [SEMICOLON] = "SEMICOLON",
};


int main(void)
{
    int token;
    while(token = get_token(stdin)) {
        printf("=> %s \n", TOKENS[token]);
    }

    return 0;
}
