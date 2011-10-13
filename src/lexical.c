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
#include "string.h"


/// Stavy konecneho automatu
enum {
    FSM_ALPHA,
    FSM_COMMA,
    FSM_DASH,
    FSM_DIV,
    FSM_DOT,
    FSM_EQUALS,
    FSM_EXP,
    FSM_FLOAT,
    FSM_GREAT,
    FSM_IDENTIFIER,
    FSM_LBRAC,
    FSM_LESS,
    FSM_MINUS,
    FSM_MUL,
    FSM_NUMBER,
    FSM_OPERATOR,
    FSM_PLUS,
    FSM_POWER,
    FSM_RBRAC,
    FSM_READ,
    FSM_SIGNED_EXP,
    FSM_START,
    FSM_STRING,
    FSM_SEMICOLON,
    FSM_TERMINATE,
    FSM_TILDA,
} TFSMStates;


/// Lexikalni analyzator
int get_token(FILE *input, string *value)
{
    static int c;
    static int state = FSM_READ;

    while(state != FSM_TERMINATE) {

        switch(state) {
            // nacitani znaku
            case FSM_READ: 
                c = fgetc(input);
            // nekdy byva potreba nacteni znaku preskocit
            case FSM_START: 
                if(isspace(c))
                    state = FSM_READ;
                else if(isalpha(c) || c == '_')
                    state = FSM_ALPHA;
                else if(isdigit(c))
                    state = FSM_NUMBER;
                else if(c == '+')
                    state = FSM_PLUS;
                else if(c == '*')
                    state = FSM_MUL;
                else if(c == '/')
                    state = FSM_DIV;
                else if(c == '.')
                    state = FSM_DOT;
                else if(c == '<')
                    state = FSM_LESS;
                else if(c == '>')
                    state = FSM_GREAT;
                else if(c == '~')
                    state = FSM_TILDA;
                else if(c == '^')
                    state = FSM_POWER;
                else if(c == '(')
                    state = FSM_LBRAC;
                else if(c == ')')
                    state = FSM_RBRAC;
                else if(c == ',')
                    state = FSM_COMMA;
                else if(c == '-')
                    state = FSM_DASH;
                else if(c == '=')
                    state = FSM_EQUALS;
                else if(c == '"')
                    state = FSM_STRING;
                else if(c == ';')
                    state = FSM_SEMICOLON;
                else if(c == EOF)
                    state = FSM_TERMINATE;
                else
                    return ERROR;
                break;

            case FSM_ALPHA: // imho ok
                str_push(value, c);
                state = FSM_IDENTIFIER;
                break;


            case FSM_IDENTIFIER: // imho ok
                c = fgetc(input);
                if(isalnum(c) || c == '_')
                    str_push(value, c);
                else {
                    state = FSM_START;
                    // TODO kontrola klicovych slov
                    return IDENTIFIER;
                }
                break;

            case FSM_NUMBER: // imho OK
                str_push(value, c);
                c = fgetc(input);
                if(isdigit(c))
                    ;
                else if(c == '.')
                    state = FSM_FLOAT;
                else if(c == 'E' || c == 'e')
                    state = FSM_EXP;
                else {
                    state = FSM_START;
                    return NUMBER;
                }
                break;

            case FSM_FLOAT: // imho OK
                str_push(value, c);
                c = fgetc(input);
                if(isdigit(c))
                    ;
                else if(c == 'E' || c == 'e')
                    state = FSM_EXP;
                else {
                    state = FSM_START;
                    return NUMBER;
                }
                break;

            case FSM_EXP:
                if(c == '+' || c == '-')
                    ;
                break;

            case FSM_SIGNED_EXP:
                break;

            case FSM_OPERATOR: // moc obecne
                str_push(value, c);
                state = FSM_READ;
                return OPERATOR;
                break;
            
            case FSM_STRING:
                return STRING;
                break;

            case FSM_SEMICOLON: // imho ok
                state = FSM_READ;
                return SEMICOLON;
                break;

            case FSM_TERMINATE:
                return NOTHING;
                break;

            default:
                printf("Not yet implemented! \n");
                state = FSM_READ;
                break;
        }
    }

    assert(0);
    return NOTHING;
}
