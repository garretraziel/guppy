/*
 * projekt: interpret jazyka IFJ11 
 * soubor: ial.c
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#include <stdlib.h>
#include <stdio.h>
#include "string.h"

#include "expr.h"
#include "defines.h"
#include "lexical.h"

extern int token;
extern string str;
extern FILE *input;


// Symboly se kterymi pracuje zasobnikovy automat
enum {
    E_POW,
    E_MUL,
    E_DIV,
    E_PLUS,
    E_MINUS,
    E_STRCONCAT,
    E_LESS,
    E_GREAT,
    E_LESSEQ,
    E_GREATEQ,
    E_NOTEQ,
    E_EQUAL,
    E_IDENT,
    E_NUM,
    E_STR,
    E_BOOL,
    E_NIL,
    E_LBRAC,
    E_RBRAC,
    E_COMMA,
    E_DOLLAR,
/*    E_MOD,
    E_STRLEN,
    E_AND,
    E_OR,
    E_NOT, */ // Tohle je do rozsireni, zatim nic

    // Znacka zacatku handle ( < do zasobniku )
    E_MARK, 

    // jeste jsou potreba neterminaly
    E_NET_E,
    E_NET_P,
} ESymbols;


// Prekladova tabulka, pokud se nacte token, tak tady muze mit jiny vyznam
const int translatetoken[] = {
// nektere tokeny primo odpovidaji symbolu do zasobniku
    [NUMBER] = E_NUM,
    [STRING] = E_STR,
    [IDENTIFIER] = E_IDENT,
    [PLUS] = E_PLUS,
    [MINUS] = E_MINUS,
    [DIV] = E_DIV,
    [MUL] = E_MUL,
    [POWER] = E_POW,
    [STRCONCAT] = E_STRCONCAT,
    [LESS] = E_LESS,
    [GREAT] = E_GREAT,
    [LESS_EQ] = E_LESSEQ,
    [GREAT_EQ] = E_GREATEQ,
    [EQUAL] = E_EQUAL,
    [NOT_EQUAL] = E_NOTEQ,
    [COMMA] = E_COMMA,
    [LBRAC] = E_LBRAC,
    [RBRAC] = E_RBRAC,    
    [FALSE] = E_BOOL,
    [TRUE] = E_BOOL,
    [NIL] = E_NIL,
// z ostatnich jsou ukoncovace
    [NOTHING] = E_DOLLAR,
    [ASSIGN] = E_DOLLAR,
    [SEMICOLON] = E_DOLLAR,
    [DO] = E_DOLLAR,
    [ELSE] = E_DOLLAR,
    [END] = E_DOLLAR,
    [FUNCTION] = E_DOLLAR,
    [IF] = E_DOLLAR,
    [LOCAL] = E_DOLLAR,
    [READ] = E_DOLLAR,
    [RETURN] = E_DOLLAR,
    [THEN] = E_DOLLAR,
    [WHILE] = E_DOLLAR,
    [WRITE] = E_DOLLAR,
    [BREAK] = E_DOLLAR,
    [ELSEIF] = E_DOLLAR,
    [FOR] = E_DOLLAR,
    [IN] = E_DOLLAR,
    [REPEAT] = E_DOLLAR,
    [UNTIL] = E_DOLLAR,
    [AND] = E_DOLLAR, // TODO ROZSIRENI
    [OR] = E_DOLLAR, // TODO ROZSIRENI
    [NOT] = E_DOLLAR, // TODO ROZSIRENI
    // strlen a modulo vubec nemaji token
};


// Precedencni tabulka
int prec_table[][E_MARK] = { // ma sirku poctu symbolu, posledni je znacka <
       /*    ^   *   /   +   -   ..  <   >   <=  >=  ~=  ==  id  nu  st  bl  ni  (   )   ,   $   */
/* ^    */ { LT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, LT, LT, OO, OO, OO, LT, GT, GT, GT, },
/* *    */ { LT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, LT, LT, OO, OO, OO, LT, GT, GT, GT, },
/* /    */ { LT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, LT, LT, OO, OO, OO, LT, GT, GT, GT, },
/* +    */ { LT, LT, LT, GT, GT, GT, GT, GT, GT, GT, GT, GT, LT, LT, OO, OO, OO, LT, GT, GT, GT, },
/* -    */ { LT, LT, LT, GT, GT, GT, GT, GT, GT, GT, GT, GT, LT, LT, OO, OO, OO, LT, GT, GT, GT, },
/* ..   */ { LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, GT, GT, LT, OO, LT, OO, OO, LT, GT, GT, GT, },
/* <    */ { LT, LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, GT, LT, LT, LT, OO, OO, LT, GT, GT, GT, },
/* >    */ { LT, LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, GT, LT, LT, LT, OO, OO, LT, GT, GT, GT, },
/* <=   */ { LT, LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, GT, LT, LT, LT, OO, OO, LT, GT, GT, GT, },
/* >=   */ { LT, LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, GT, LT, LT, LT, OO, OO, LT, GT, GT, GT, },
/* ~=   */ { LT, LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, GT, LT, LT, LT, OO, OO, LT, GT, GT, GT, },
/* ==   */ { LT, LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, GT, LT, LT, LT, OO, OO, LT, GT, GT, GT, },
/* id   */ { GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, OO, OO, OO, OO, OO, EQ, GT, GT, GT, },
/* num  */ { GT, GT, GT, GT, GT, OO, GT, GT, GT, GT, GT, GT, OO, OO, OO, OO, OO, OO, GT, GT, GT, },
/* str  */ { OO, OO, OO, OO, OO, GT, GT, GT, GT, GT, GT, GT, OO, OO, OO, OO, OO, OO, GT, GT, GT, },
/* bool */ { OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, GT, GT, GT, },
/* nil  */ { OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, GT, GT, GT, },
/* (    */ { LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, EQ, LT, OO, },
/* )    */ { GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, OO, OO, OO, OO, OO, OO, GT, OO, GT, },
/* ,    */ { LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, OO, GT, LT, GT, },
/* $    */ { LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, OO, LT, OO, },
/* %    */
/* #    */
/* and  */
/* or   */
/* not  */
};



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Polozka zasobniku
typedef struct node {
    int type;
    void *res; // zrejme bude treba mit tam nejaka dalsi data
    struct node *next;
} Node; // Expression Stack Node

typedef struct {
    Node *top;
    Node *active;
} Stack;

// Inicializace zasobniku
int s_init(Stack *stack)
{
    stack->top = NULL;
    stack->active = NULL;
}

// Pridani symbolu na zasobnik (to se hodi vzdycky)
int s_push(Stack *stack, int t)
{
    Node *new = malloc(sizeof(Node));
    if(new == NULL)
        return 0;
    new->type = t;
    new->res = NULL;
    new->next  = stack->top;
    stack->top = new;
    if( /* t je terminal */ 0 )
        stack->active = new;
    return 1;
}

// Zamena symbolu x za x< na zasobniku (reakce na <)
int s_alter(Stack *stack, int t)
{
    return 1;
}

// Aplikace pravidla (reakce na >)
int s_oobely_boo(Stack *stack, int t)
{
    // potreba najit nejake pravidlo a nahradit to, to bude jeste svanda
    return 1;
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int expression(void)
{
    int a, b;
    Stack stack;
    s_init(&stack);

    // Takhle to zacina, da se dolar na zasobnik
    s_push(&stack, E_DOLLAR);

    do {
        a = translatetoken[token];  // aktualni vstup
        b = stack.active->type;    // nejvrchnejsi terminal na zasobniku
        switch( prec_table[b][a] ) {
            case EQ:
                // push(a)
                s_push(&stack, a);
                // precist novy token
                get_token(input, &str);
                a = translatetoken[token];
                break;

            case LT:
                // b na zasobniku vymenit za b<
                // push(a)
                s_push(&stack, a);
                // precist novy token
                get_token(input, &str);
                a = translatetoken[token];
                break;

            case GT:
                // aplikace pravidla, tohle bude nejslozitejsi
                // pokud je na zasobiku <y a existuje pravidlo r: A -> y
                    // pak vymenit <y za A
                    // a pouzit to pravidlo
                // jinak chyba
                break;

            case OO:
            default:
                // jinak chyba
                return ERROR_SEM_X;
                break;
        } /* switch */
    } while(a != E_DOLLAR || b != E_DOLLAR);

    // asi jeste poklidit zasobnicek
    // TODO: pokud se vraci chyba, mel by se taky poklidit zasobnik atd.
    // pouzijem goto at to stoji za to?

    return 0;
}
