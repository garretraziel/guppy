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
    FSM_COMMENT_LINE,
    FSM_COMMENT_BLOCK,
    FSM_COMMENT_BLOCK_END,
    FSM_DASH,
    FSM_DASH2,
    FSM_DASH2B,
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
    FSM_LESS,
    FSM_NOT_EQUALS,
    FSM_NUMBER,
    FSM_READ,
    FSM_START,
    FSM_STRING,
    FSM_TERMINATE,
} TFSMStates;

const char *LEX_ERRORS[] = {
    [NOTHING] = "NULL",
    [-ERROR_X_DIGIT] = "Ocekavana cislice",
    [-ERROR_UX_CHAR] = "Neocekavany znak",
    [-ERROR_X_SIGNDIGIT] = "Ocekavano znaminko nebo cislice",
    [-ERROR_ESC_SEC] = "Neplatna escape sekvence",
    [-ERROR_OPERATOR] = "Neplatny operator",
};

// pocitadlo radku
int line = 1;

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
                if(isspace(c)) {
                    state = FSM_READ;
                    if(c == '\n')
                        ++line;
                }
                else if(isalpha(c) || c == '_')
                    state = FSM_IDENTIFIER;
                else if(isdigit(c))
                    state = FSM_NUMBER;
                else if(c == '+') {
                    state = FSM_READ;
                    return PLUS;
                }
                else if(c == '*') {
                    state = FSM_READ;
                    return MUL;
                }
                else if(c == '/') {
                    state = FSM_READ;
                    return DIV;
                }
                else if(c == '.')
                    state = FSM_DOT;
                else if(c == '<')
                    state = FSM_LESS;
                else if(c == '>')
                    state = FSM_GREAT;
                else if(c == '~')
                    state = FSM_NOT_EQUALS;
                else if(c == '^') {
                    state = FSM_READ;
                    return POWER;
                }
                else if(c == '(') {
                    state = FSM_READ;
                    return LBRAC;
                }
                else if(c == ')') {
                    state = FSM_READ;
                    return RBRAC;
                }
                else if(c == ',') {
                    state = FSM_READ;
                    return COMMA;
                }
                else if(c == '-')
                    state = FSM_DASH;
                else if(c == '=')
                    state = FSM_EQUALS;
                else if(c == '"')
                    state = FSM_STRING;
                else if(c == ';') {
                    state = FSM_READ;
                    return SEMICOLON;
                }
                else if(c == EOF)
                    return NOTHING;
                else
                    return ERROR_UX_CHAR;
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
                    return ERROR_X_DIGIT;
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
                    return ERROR_X_SIGNDIGIT;
                }
                break;

            case FSM_EXP0:
                str_push(value, c);
                c = fgetc(input);
                if(isdigit(c))
                    state = FSM_EXP;
                else {
                    state = FSM_START;
                    return ERROR_X_DIGIT;
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

// NOTE: Zminit v dokumentaci, ze jsme zakazali primo vkladat ridici znaky ASCII do retezcu

            case FSM_STRING:
                c = fgetc(input);
                if(c < ASCII_CONTROLL) {
                    state = FSM_STRING; // TODO: zvazit, kam to ma pokracovat
                    return ERROR_UX_CHAR;
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
                    return ERROR_ESC_SEC;
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
                    return ERROR_ESC_SEC;
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
                return ERROR_ESC_SEC;
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
                if(c == '\n') {
                    state = FSM_READ;
                    ++line;
                }
                break;

            case FSM_COMMENT_BLOCK:
                c = fgetc(input);
                if(c == ']')
                    state = FSM_COMMENT_BLOCK_END;
                else if(c == '\n')
                    ++line;
                break;

            case FSM_COMMENT_BLOCK_END:
                c = fgetc(input);
                if(c == ']')
                    state = FSM_READ;
                else {
                    state = FSM_COMMENT_BLOCK;
                    if(c == '\n')
                        ++line;
                }
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
                    return ERROR_OPERATOR;
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
                    return ERROR_OPERATOR;
                }
                break;
        }
    }

    assert(0);
    return NOTHING;
}
