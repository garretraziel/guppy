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
    FSM_COMMA,
    FSM_COMMENT_LINE,
    FSM_COMMENT_BLOCK,
    FSM_COMMENT_BLOCK_END,
    FSM_DASH,
    FSM_DASH2,
    FSM_DASH2B,
    FSM_DIV,
    FSM_DOT,
    FSM_EQUALS,
    FSM_ESCAPE,
    FSM_ESCAPE_NUM,
    FSM_ESCAPE_NUM2,
    FSM_EXP,
    FSM_EXP0,
    FSM_EXP_E,
    FSM_FLOAT,
    FSM_FLOAT0,
    FSM_GREAT,
    FSM_IDENTIFIER,
    FSM_LBRAC,
    FSM_LESS,
    FSM_MUL,
    FSM_NOT_EQUALS,
    FSM_NUMBER,
    FSM_PLUS,
    FSM_POWER,
    FSM_RBRAC,
    FSM_READ,
    FSM_START,
    FSM_STRING,
    FSM_SEMICOLON,
    FSM_TERMINATE,
} TFSMStates;


/// Lexikalni analyzator
int get_token(FILE *input, string *value)
{
    static int c;
    static int state = FSM_READ;

    int num;

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
                    state = FSM_IDENTIFIER;
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
                    state = FSM_NOT_EQUALS;
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

            case FSM_IDENTIFIER:
                str_push(value, c);
                c = fgetc(input);
                if(! (isalnum(c) || c == '_')) {
                    state = FSM_START;
                    // TODO kontrola klicovych slov
                    return IDENTIFIER;
                }
                break;

            case FSM_NUMBER:
                str_push(value, c);
                c = fgetc(input);
                if(isdigit(c))
                    ;
                else if(c == '.')
                    state = FSM_FLOAT0;
                else if(c == 'E' || c == 'e')
                    state = FSM_EXP_E;
                else {
                    state = FSM_START;
                    return NUMBER;
                }
                break;

            case FSM_FLOAT0:
                str_push(value, c);
                c = fgetc(input);
                if(isdigit(c))
                    state = FSM_FLOAT;
                else {
                    state = FSM_START;
                    return ERROR;
                }
                break;
            
            case FSM_FLOAT:
                str_push(value, c);
                c = fgetc(input);
                if(isdigit(c))
                    ;
                else if(c == 'E' || c == 'e')
                    state = FSM_EXP_E;
                else {
                    state = FSM_START;
                    return NUMBER;
                }
                break;

            case FSM_EXP_E:
                str_push(value, c);
                c = fgetc(input);
                if(c == '+' || c == '-')
                    state = FSM_EXP0;
                else if (isdigit(c))
                    state = FSM_EXP;
                else { 
                    state = FSM_START;
                    return ERROR;
                }
                break;

            case FSM_EXP0:
                str_push(value, c);
                c = fgetc(input);
                if(isdigit(c))
                    state = FSM_EXP;
                else {
                    state = FSM_START;
                    return ERROR;
                }
                break;


            case FSM_EXP:
                str_push(value, c);
                c = fgetc(input);
                if(! isdigit(c)) {
                    state = FSM_START;
                    return NUMBER;
                }
                break;

            case FSM_STRING:
                c = fgetc(input);
                if(c < 1) {
                    state = FSM_STRING; // TODO: zvazit, kam to ma pokracovat
                    return ERROR;
                } else if(c == '\\')
                    state = FSM_ESCAPE;
                else if(c == '"') {
                    state = FSM_READ;
                    return STRING;
                } else
                    str_push(value, c);
                break;

            case FSM_ESCAPE:
                c = fgetc(input);
                if(c == 'n') {
                    state = FSM_STRING;
                    str_push(value, '\n');
                } else if(c == 't') {
                    state = FSM_STRING;
                    str_push(value, '\t');
                } else if(c == '\\' || c == '"') {
                    state = FSM_STRING;
                    str_push(value, c);
                } else if(isdigit(c))
                    state = FSM_ESCAPE_NUM;
                else {
                    state = FSM_STRING; // TODO: zvazit, kam to ma pokracovat
                    return ERROR;
                }
                break;

// TODO: tu ciselnou escape sekvenci vymyslet lepe

            case FSM_ESCAPE_NUM:
                num = c - '0';
                c = fgetc(input);
                if(isdigit(c)) { 
                    num = num * 10 + c - '0';
                    state = FSM_ESCAPE_NUM2;
                }
                else {
                    state = FSM_STRING; // TODO: zvazit, kam to ma pokracovat
                    return ERROR;
                }
                break;

            case FSM_ESCAPE_NUM2: // TODO
                c = fgetc(input);
                if(isdigit(c)) { 
                    num = num * 10 + c - '0';
                    if(num > 0 && num < 256) {
                        str_push(value, num);
                        state = FSM_STRING;
                        break;
                    }
                }
                state = FSM_STRING; // TODO: zvazit, kam to ma pokracovat
                return ERROR;
                break;

            case FSM_DASH:
                c = fgetc(input);
                if(c == '-')
                    state = FSM_DASH2;
                else {
                    state = FSM_START;
                    return MINUS;
                }
                break;

            case FSM_DASH2:
                c = fgetc(input);
                if(c == '[')
                    state = FSM_DASH2B;
                else if(c == '\n')
                    state = FSM_READ;
                else
                    state = FSM_COMMENT_LINE;
                break;

            case FSM_DASH2B:
                c = fgetc(input);
                if(c == '[')
                    state = FSM_COMMENT_BLOCK;
                else
                    state = FSM_COMMENT_LINE;
                break;

            case FSM_COMMENT_LINE:
                c = fgetc(input);
                if(c == '\n')
                    state = FSM_READ;
                break;

            case FSM_COMMENT_BLOCK:
                c = fgetc(input);
                if(c == ']')
                    state = FSM_COMMENT_BLOCK_END;
                break;

            case FSM_COMMENT_BLOCK_END:
                c = fgetc(input);
                if(c == ']')
                    state = FSM_READ;
                else
                    state = FSM_COMMENT_BLOCK;
                break;

            case FSM_EQUALS:
                c = fgetc(input);
                if(c == '=') {
                    state = FSM_READ;
                    return EQUAL;
                } else {
                    state = FSM_START;
                    return ASSIGN;
                }
                break;

            case FSM_DOT:
                c = fgetc(input);
                if(c == '.') {
                    state = FSM_READ;
                    return STRCONCAT;
                } else {
                    state = FSM_START;
                    return ERROR;
                }
                break;

            case FSM_LESS:
                c = fgetc(input);
                if(c == '=') {
                    state = FSM_READ;
                    return LESS_EQ;
                } else {
                    state = FSM_START;
                    return LESS;
                }
                break;

            case FSM_GREAT:
                c = fgetc(input);
                if(c == '=') {
                    state = FSM_READ;
                    return GREAT_EQ;
                } else {
                    state = FSM_START;
                    return GREAT;
                }
                break;

            case FSM_NOT_EQUALS:
                c = fgetc(input);
                if(c == '=') {
                    state = FSM_READ;
                    return NOT_EQUAL;
                } else {
                    state = FSM_START;
                    return ERROR;
                }
                break;

// TODO: zkusit nejak smrsknout kod nasledujicich stavu:

            case FSM_SEMICOLON:
                state = FSM_READ;
                return SEMICOLON;
                break;

            case FSM_PLUS:
                state = FSM_READ;
                return PLUS;
                break;

            case FSM_DIV:
                state = FSM_READ;
                return DIV;
                break;

            case FSM_MUL:
                state = FSM_READ;
                return MUL;
                break;

            case FSM_POWER:
                state = FSM_READ;
                return POWER;
                break;

            case FSM_LBRAC:
                state = FSM_READ;
                return LBRAC;
                break;

            case FSM_RBRAC:
                state = FSM_READ;
                return RBRAC;
                break;

            case FSM_COMMA:
                state = FSM_READ;
                return COMMA;
                break;

            case FSM_TERMINATE:
                return NOTHING;
                break;

            default:
                printf("Not yet implemented! [%c]\n", state);
                state = FSM_READ;
                break;
        }
    }

    assert(0);
    return NOTHING;
}
