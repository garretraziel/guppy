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
    E_POW, //0 

    E_MUL,
    E_DIV, //2
    E_PLUS,
    E_MINUS, //4

    E_STRCONCAT,

    E_LESS, //6
    E_GREAT,
    E_LESSEQ, //8
    E_GREATEQ,
    E_NOTEQ, //10
    E_EQUAL,

    E_IDENT, //12
    E_NUM,
    E_STR,  // 14
    E_BOOL,
    E_NIL,  // 16
    
    E_LBRAC,           
    E_RBRAC, // 18
    E_COMMA,
    E_DOLLAR, //20
/*    E_MOD,
    E_STRLEN,
    E_AND,
    E_OR,
    E_NOT, */ // Tohle je do rozsireni, zatim nic

    // Znacka zacatku handle ( < do zasobniku )
    E_MARK, //21

    // jeste jsou potreba neterminaly
    E_NET_E,
    E_NET_P,
} ESymbols;

#define is_terminal(e_sym)(e_sym < E_MARK)

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
/* ~=   */ { LT, LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, GT, LT, LT, LT, LT, LT, LT, GT, GT, GT, },
/* ==   */ { LT, LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, GT, LT, LT, LT, LT, LT, LT, GT, GT, GT, },
/* id   */ { GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, OO, OO, OO, OO, OO, EQ, GT, GT, GT, },
/* num  */ { GT, GT, GT, GT, GT, OO, GT, GT, GT, GT, GT, GT, OO, OO, OO, OO, OO, OO, GT, GT, GT, },
/* str  */ { OO, OO, OO, OO, OO, GT, GT, GT, GT, GT, GT, GT, OO, OO, OO, OO, OO, OO, GT, GT, GT, },
/* bool */ { OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, GT, GT, OO, OO, OO, OO, OO, OO, GT, GT, GT, },
/* nil  */ { OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, GT, GT, OO, OO, OO, OO, OO, OO, GT, GT, GT, },
/* (    */ { LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, EQ, LT, OO, },
/* )    */ { GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, OO, OO, OO, OO, OO, OO, GT, GT, GT, },
/* ,    */ { LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, GT, },
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
    
    return 1;
}

//smazani zasobniku
int s_clean(Stack * stack)
{
    Node * tmp;
    while(stack->top != NULL){
        tmp = stack->top;
        stack->top = tmp->next;
        free(tmp);
    }
    return 1;
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
    if(is_terminal(t))
        stack->active = new;
    return 1;
}

void s_pop(Stack *stack){
    Node *tmp = stack->top;
    stack->top = tmp->next;
    free(tmp);
    tmp = stack->top;
    while(!is_terminal(tmp->type)){
        tmp = tmp->next;
    }
    stack->active = tmp;
}

// Zamena symbolu x za x< na zasobniku (reakce na <)
static int s_alter(Stack *stack)
{
    Node * new = malloc(sizeof(Node));
    if(new == NULL)
        return 0;

    new->type = stack->active->type;
    new->res = NULL;
    new->next = stack->active->next;
    stack->active->next = new;
 //   if(stack->active == stack->top){
 //       stack->top = new;
 //   }
    stack->active->type = E_MARK;
    //na chvilu se rozbije active
    return 1;
}

//jen pro debug
void s_dump(Stack *stack)
{
        Stack s_tmp;
        s_init(&s_tmp);
        char * type[] = {"^","*","/","+","-","..","<",">","<=",">=","~=","==",
                         "i","n","s","b","nil","(",")", ",","$", "{","E","P"};
        Node * tmp = stack->top;
        while(tmp!=NULL){
            s_push(&s_tmp, tmp->type);
            tmp = tmp->next;
        }
        tmp = s_tmp.top;
        while(tmp!=NULL){
            printf(" %s ",type[tmp->type]);
            tmp = tmp->next;
        }
        s_clean(&s_tmp);
        printf("\n");
}

// Aplikace pravidla (reakce na >)
static int s_oobely_boo(Stack *stack)
{
    // potreba najit nejake pravidlo a nahradit to, to bude jeste svanda
    
    /* Pravidla bez volani fci: 
     * 1: E -> i //i je z {E_NUM, E_STR, E_BOOL, E_IDENT, E_NIL}
     * 2: E -> (E)
     * 3: E -> E^E
     * 4: E -> E * E
     * 5: E -> E / E
     * 6: E -> E + E
     * 7: E -> E - E
     * 8: E -> E .. E
     * 9: E -> E < E
     *10: E -> E > E
     *11: E -> E <= E
     *12: E -> E >= E
     *13: E -> E == E
     *14: E -> E ~= E
     *15: E -> E_IDENT ( )
     *16: E -> E_IDENT ( E P
     *17: P -> )
     *18: P -> , E P
     */
    enum stavy{
    _OKAY=-1,
    _ERR=0,   
    _START,  
    _FIRST_E, 
    _OP,
    _F_OPEN,
    };
    int state = 1;
    int op = 0;
    if(stack->top->type == E_DOLLAR) {
        //toto tady asi nebude
        return 0;
    }
    while(stack->top->type != E_MARK && state > _ERR){
        s_dump(stack);
        switch(stack->top->type){
            case E_STR:
                    //tady bude workaround pro string literaly
            case E_NUM:
            case E_BOOL:
            case E_IDENT:
            case E_NIL:
                if(state == _START){ // 1 je pocatecni stav
                    op = stack->top->type;
                    s_pop(stack);
                    state = _OKAY;
                    //s_push(stack, E_NET_E); //aplikace pravidla na zasobniku
                    printf("E->i\n");
                } else
                    state = _ERR; //chyba
            break;
            
            case E_NET_E:
                if(state == _START){ //jeste jsem nenacetl
                    s_pop(stack);
                    state = _FIRST_E; //prvni E nacteno
                } else
                if(state == _OP){ //operator byl nacten 
                    s_pop(stack);
                    state = _OKAY;
                    //s_push(stack, E_NET_E);
                    printf("E->E %d E\n", op);
                } else
                    state = _ERR; //chyba
            break; //KOKOT JSEM, na toto nezapominat    

            case E_LBRAC:
                if(state == _FIRST_E){
                    op = stack->top->type;
                    s_pop(stack);
                    state = _OKAY; //koncovy stav
                    //s_push(stack, E_NET_E); // E->(E)
                    printf("E->(E)\n");
                } else 
                    state = _ERR; //chyba
            break;

            default: //ostatni terminaly
                if(state == _FIRST_E){ // jen kdyz uz jsem nacetl jeden neterminal
                    op = stack->top->type; //ulozim si operator
                    s_pop(stack);
                    state = _OP;
                } else { 
                    state = _ERR;
                }
            break;
            
        }
        
    }

    if(state == _OKAY && stack->top->type == E_MARK){
        s_pop(stack); //odstrani < ze zasobniku
           
        switch(op){
            case E_IDENT:
            case E_NUM:
            case E_STR:
            case E_BOOL:
            case E_NIL:
                s_push(stack, E_NET_E);
                s_dump(stack);
                return op; //pravidlo 1: E->i
            break; //asi zbytecne

            default:
                s_push(stack, E_NET_E);
                s_dump(stack);
                return op; //vraci pravdilo 2 - 14
            break;
        }
        
    } else {
        return -1;
    }
}





/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int expression(void)
{
    int a, b;
    Stack stack;
    s_init(&stack);

    // radsi kontrola, on to nikdo predtim asi nedela a to tabulky s tim nemuzu
    if(token < 0)
        return token;

    // Takhle to zacina, da se dolar na zasobnik
    s_push(&stack, E_DOLLAR);

    a = translatetoken[token]; // aktualni vstup
    do {
        b = stack.active->type; // nejvrchnejsi terminal na zasobniku
        switch( prec_table[b][a] ) {
            case EQ:
                // push(a)
                s_push(&stack, a);
                b = stack.active->type; // doslo ke zmene horniho terminalu
                // precist novy token
                get_token();
                if (token < 0) { // nacetl jsem neco spatneho
                    s_clean(&stack);
                    return token;
                }
                a = translatetoken[token];
                break;

            case LT:
                // b na zasobniku vymenit za b<
                s_alter(&stack);
                // push(a)
                s_push(&stack, a);
                b = stack.active->type; // doslo ke zmene horniho terminalu
                // precist novy token
                get_token();
                if (token < 0) { // nacetl jsem neco spatneho
                    s_clean(&stack);
                    return token;
                }
                a = translatetoken[token];
                break;

            case GT:
                // aplikace pravidla, tohle bude nejslozitejsi
                // pokud je na zasobiku <y a existuje pravidlo r: A -> y
                    // pak vymenit <y za A
                    // a pouzit to pravidlo
                // na tohle melo byt puvodne oobely_boo
                if(s_oobely_boo(&stack) != -1)
                    b = stack.active->type; //potreba pro ukonceni cyklu
                else {
                    s_clean(&stack);
                    return ERROR_SYN_EXP_FAIL;
                }
                // jinak chyba
                break;

            case OO:
            default:
                // pokud je neocekavanym tokenem carka nebo zavorka,
                // tak se rekne, ze je konec vyrazu, kvuli write(E , E)
                if(token == COMMA || token == RBRAC) {
                    a = E_DOLLAR;
                    continue;
                }
                // jinak syntakticka chyba
                s_clean(&stack);
                return ERROR_SYN_EXP_FAIL;
                break;
        } /* switch */
    } while(a != E_DOLLAR || b != E_DOLLAR);

    // uvolneni zasobniku, vzdycky tam zbyde E_DOLLAR
    s_clean(&stack);
    return 1;
}
