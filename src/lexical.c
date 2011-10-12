/*
 * projekt: interpret jazyka IFJ11 
 * soubor: lexical.c
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#include <stdio.h>
#include <ctype.h>

#include <assert.h>

#include "guppy.h"
#include "lexical.h"

enum {
    FSM_START,
    FSM_READ,
    FSM_TERMINATE,
    FSM_IDENTIFIER,
    FSM_NUMBER,
    FSM_OPERATOR,
    FSM_STRING,
    FSM_SEMICOLON,
    FSM_NYI,
} TFSMStates;


/* Bude potreba buffer na stringy a cisla
 * kde se uchova hodnota, na konci prevede (cislo),
 * nebo prekopiruje (string) optimalne velikeho pametoveho prostoru
 * 
 *
 * pro zacatek misto bufferu se:
 *  nejdrive vypise [
 *  pak uzitecny obsah tokenu
 *  nakonec ] a return
 */


/// Lexikalni analyzator
int get_token(FILE *input)
{
    static int c;
    static int state = FSM_READ;

    // TODO Potrebujem buffer, jak ho udelame?

    while(state != FSM_TERMINATE) {
        switch(state) {
            // pokud potrebuji nacist znak pro zacatek
            case FSM_READ: 
                c = fgetc(input);
            // pokud uz ho nacteny mam
            case FSM_START: 
                if(isspace(c))
                    state = FSM_READ;
                else if(isalpha(c) || c == '_')
                    state = FSM_IDENTIFIER;
                else if(isdigit(c))
                    state = FSM_NUMBER;
                else if(c == '+' || c == '-' || c == '*' || c == '/')
                    state = FSM_OPERATOR;
                else if(c == '=')
                    state = FSM_NYI;
                else if(c == '"')
                    state = FSM_STRING;
                else if(c == ';')
                    state = FSM_SEMICOLON;
                else if(c == EOF) {
                    state = FSM_TERMINATE;
                }
                break;

            case FSM_IDENTIFIER:
                printf("[");
                do {
                    putchar(c);
                    // ukladani do bufferu
                } while((c = getchar()) != EOF && (isalnum(c) || c == '_'));
                printf("]");
                state = FSM_START;
                return IDENTIFIER;
                break;

            case FSM_NUMBER:
                printf("[");
                do {
                    putchar(c);
                } while((c = getchar()) != EOF && isdigit(c));
                state = FSM_START;
                printf("]");
                return NUMBER;
                break;

            case FSM_OPERATOR:
                printf("[%c]", c);
                state = FSM_READ;
                return OPERATOR;
                break;
            
            case FSM_STRING:
                printf("[");
                while((c = getchar()) != EOF && c != '"') {
                    putchar(c);
                    // escape sekvence
                    // ukladani do bufferu
                }
                printf("]");

                // konci to pri EOF a ", takze EOF je chyba
                if(c == EOF)
                    state = FSM_TERMINATE;
                else
                    state = FSM_READ;
                return STRING;
                break;

            case FSM_NYI:
                printf("--> [%c]\n", c);
                state = FSM_TERMINATE;
                break;

            case FSM_SEMICOLON:
                printf("[%c]", c);
                state = FSM_READ;
                return SEMICOLON;
                break;

            case FSM_TERMINATE:
                return NOTHING;
                break;
        }
    }

    assert(0);
    return NOTHING;
}
