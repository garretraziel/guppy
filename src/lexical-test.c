#include <stdio.h>

#include "lexical.h"


const char *TOKENS[] = {
    [NOTHING] = "NULL",
    [NUMBER] = "NUMBER",
    [STRING] = "STRING",
    [IDENTIFIER] = "IDENTIFIER",
    [SEMICOLON] = "SEMICOLON",

    [PLUS] = "PLUS",
    [MINUS] = "MINUS",
    [DIV] = "DIV",
    [MUL] = "MUL",
    [POWER] = "POWER",
    [STRCONCAT] = "STRCONCAT",

    [ASSIGN] = "ASSIGN",
    [LESS] = "LESS",
    [GREAT] = "GREAT",
    [LESS_EQ] = "LESS_EQ",
    [GREAT_EQ] = "GREAT_EQ",
    [TILDA] = "TILDA",

    [COMMA] = "COMMA",
    [LBRAC] = "LBRAC",
    [RBRAC] = "RBRAC",    

    [FUNCTION] = "FUNCTION",
    [END] = "END",
};


int main(void)
{
    int token;

    string data;

    str_new(&data, STR_DEFAULT);

    while((token = get_token(stdin, &data))) {
        printf("%s [%s] \n", TOKENS[token], data.str);
        str_clean(&data);
    }

    return 0;
}
