#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "lexical.h"
#include "defines.h"


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

void Error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "CHYBA: ");
    vfprintf(stderr, fmt, args);
    va_end(args);
}

int main(void)
{
    int token;

    str_new(&str, STR_DEFAULT);

    input = stdin;

    while((token = get_token())) {
        if(token > 0) {
            printf("%s [%s] \n", TOKENS[token], str.str);
            if(token == NUMBER)
                printf("-> %lg\n", strtod(str.str, NULL));
        }
        else
            Error("Na radku %d: %s\n", line, ERROR_MSG[(-token)/100][(-token)%100]);
        str_clean(&str);
    }

    str_free(&str);

    return 0;
}
