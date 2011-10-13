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

    string data;

    while((token = get_token(stdin, &data))) {
        printf("%s [%s] \n", TOKENS[token], data.str);
    }

    return 0;
}
