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



int expression(void)
{
    Stack stack;

    // TODO: mozna je potreba prepocitavat token do podoby, jakou papa tohle
    // protoze tohle potrebuje mit i reprezentaci < do stacku
    // a taky neterminalu
    // to si jeste uzijem

    int a; // aktualni vstup
    int b; // nejvrchnejsi terminal na zasobniku

    s_init(&stack);

    // Takhle to zacina, da se dolar na zasobnik
    s_push(&stack, /* DOLAR */ 0);

    // veliky cyklus
    do {
        switch( /*tabulka[a][b]*/ 0 ) {
            case '=':
                s_push(&stack, a);
                get_token(input, &str);
                a = token;
                break;
            case '<':
                // s_alter
                s_push(&stack, a);
                get_token(input, &str);
                a = token;
                break;
            case '>':
                // aplikace pravidla, tohle bude nejslozitejsi
                break;
            default:
                return ERROR_SEM_X;
                break;
        }
    } while( 0 && ! (a == /*DOLAR*/ 0 && b == /*DOLAR*/ 0) ); // tadlenc to ma koncit

    // asi jeste poklidit zasobnicek
    return 0;
}
