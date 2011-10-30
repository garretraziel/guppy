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
    [EQUAL] = "EQUAL",
    [NOT_EQUAL] = "NOT_EQUAL",

    [COMMA] = "COMMA",
    [LBRAC] = "LBRAC",
    [RBRAC] = "RBRAC",    

    [DO] = "DO",
    [ELSE] = "ELSE",
    [END] = "END",
    [FALSE] = "FALSE",
    [FUNCTION] = "FUNCTION",
    [IF] = "IF",
    [LOCAL] = "LOCAL",
    [NIL] = "NIL",
    [READ] = "READ",
    [RETURN] = "RETURN",
    [THEN] = "THEN",
    [TRUE] = "TRUE",
    [WHILE] = "WHILE",
    [WRITE] = "WRITE",

    [AND] = "AND",
    [BREAK] = "BREAK",
    [ELSEIF] = "ELSEIF",
    [FOR] = "FOR",
    [IN] = "IN",
    [NOT] = "NOT",
    [OR] = "OR",
    [REPEAT] = "REPEAT",
    [UNTIL] = "UNTIL",
};


int main(void)
{
    int token;

    string data;

    str_new(&data, STR_DEFAULT);

    while((token = get_token(stdin, &data))) {
        if(token > 0)
            printf("%s [%s] \n", TOKENS[token], data.str);
        else
            fprintf(stderr, "CHYBA na radku %d : %s\n", line, LEX_ERRORS[-token]);
        str_clean(&data);
    }

    return 0;
}
