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
#include <string.h>

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
    [-ERROR_NUMBER] = "Neplatny ciselny literal",
    [-ERROR_UX_CHAR] = "Neocekavany znak",
    [-ERROR_ESC_SEC] = "Neplatna escape sekvence v retezci",
};

// globalni pocitadlo radku
int line = 1;


// pro potreby unget_token
static int buffer = NOTHING;
static int buffer_valid = 0; // false

/** Funkce kontroluje, jestli neni ve stringu klicove nebo rezervovane slovo
 *
 * vraci jeho pripadny kod, nebo IDENTIFIER pokud neni
 * TODO: zoptimalizovat poradi kontroly na shodu
 */
int check_keyword(string *str)
{
    if(strcmp(str->str, "do") == 0)
        return DO;
    if(strcmp(str->str, "else") == 0)
        return ELSE;
    if(strcmp(str->str, "end") == 0)
        return END;
    if(strcmp(str->str, "false") == 0)
        return FALSE;
    if(strcmp(str->str, "function") == 0)
        return FUNCTION;
    if(strcmp(str->str, "if") == 0)
        return IF;
    if(strcmp(str->str, "local") == 0)
        return LOCAL;
    if(strcmp(str->str, "nil") == 0)
        return NIL;
    if(strcmp(str->str, "read") == 0)
        return READ;
    if(strcmp(str->str, "return") == 0)
        return RETURN;
    if(strcmp(str->str, "then") == 0)
        return THEN;
    if(strcmp(str->str, "true") == 0)
        return TRUE;
    if(strcmp(str->str, "while") == 0)
        return WHILE;
    if(strcmp(str->str, "write") == 0)
        return WRITE;
    if(strcmp(str->str, "and") == 0)
        return AND;
    if(strcmp(str->str, "break") == 0)
        return BREAK;
    if(strcmp(str->str, "elseif") == 0)
        return ELSEIF;
    if(strcmp(str->str, "for") == 0)
        return FOR;
    if(strcmp(str->str, "in") == 0)
        return IN;
    if(strcmp(str->str, "not") == 0)
        return NOT;
    if(strcmp(str->str, "or") == 0)
        return OR;
    if(strcmp(str->str, "repeat") == 0)
        return REPEAT;
    if(strcmp(str->str, "until") == 0)
        return UNTIL;
    return IDENTIFIER;
}


/** Lexikalni analyzator
 *
 * @param string slozi k navratu identifikatoru a cisel
 * vraci ciselnou reprezentaci tokenu
 */
int get_token(FILE *input, string *value)
{
    static int c;
    static int state = FSM_READ;

    int num;

    // vraceni tokenu z bufferu
    if(buffer_valid) {
        buffer_valid = 0;
        return buffer;
    }

    for(;;) {

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
                    return check_keyword(value); // kontrola klicovych slov
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
                    return ERROR_NUMBER;
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
                    return ERROR_NUMBER;
                }
                break;

            case FSM_EXP0:
                str_push(value, c);
                c = fgetc(input);
                if(isdigit(c))
                    state = FSM_EXP;
                else {
                    state = FSM_START;
                    return ERROR_NUMBER;
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
                    state = FSM_START;
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
                    state = FSM_START;
                    return ERROR_ESC_SEC;
                }
                break;

            case FSM_ESCAPE_NUM:
                num = c - '0';
                c = fgetc(input);
                if(isdigit(c)) { 
                    num = num * 10 + c - '0';
                    state = FSM_ESCAPE_NUM2;
                }
                else {
                    state = FSM_STRING;
                    return ERROR_ESC_SEC;
                }
                break;

            case FSM_ESCAPE_NUM2:
                c = fgetc(input);
                if(isdigit(c)) { 
                    num = num * 10 + c - '0';
                    if(num > 0 && num < 256) {
                        str_push(value, num);
                        state = FSM_STRING;
                    }
                } else {
                    state = FSM_START;
                    return ERROR_ESC_SEC;
                }
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
                    return ERROR_UX_CHAR;
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
                    return ERROR_UX_CHAR;
                }
                break;
        }
    }

    assert(0);
    return NOTHING;
}


/** Vrati token zpet na vstup
 *
 * vzdy se smi vratit jen jeden
 * nesmi se znicit obsah nactenych dat, ty se ukladat nebudou
 */
int unget_token(int token)
{
    if(buffer_valid)
        return 0; 
    buffer_valid = 1;
    buffer = token;
    return 1;
}
