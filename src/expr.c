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
#include "exec.h"



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

    // tohle jsou jen pomocne konstanty pro oobely-boo
    E_OP, // obecne operator
    E_LIT, // obecne literal
    E_UNKNOWN, // neznamy typ
} ESymbols;



// Normalni clovek nezanori vice volani funkci nez 3, 20 musi stacit kazdemu
#define FUNC_STACK_SIZE 20


// Zasobnik poctu parametru instance volani funkce
static int func_stack[FUNC_STACK_SIZE] = {0};
// Ukazatel do zasobniku
static int F = -1;

// Nova funkce, posunout se na nove misto
#define func_push() do { if(++F >= FUNC_STACK_SIZE) return ERROR_GEN_MEM; } while(0)
// Funkce konci, vynulovat a pop
#define func_pop() do { func_stack[F] = 0; F--; } while(0)
// Pripocteni 1 do zasobniku parametru
#define func_inc() do { func_stack[F] += 1; } while(0)

// Makro vraci true, pokud je symbol terminalni
#define is_terminal(e_sym)(e_sym < E_MARK)


/*
 * Vraci typ vysledku operatoru
 */
static inline int get_result_type(int op)
{
    switch(op) {
        case E_POW:
        case E_MUL:
        case E_DIV:
        case E_PLUS:
        case E_MINUS:
            return E_NUM;
        case E_STRCONCAT:
            return E_STR;
        case E_LESS:
        case E_GREAT:
        case E_LESSEQ:
        case E_GREATEQ:
        case E_NOTEQ:
        case E_EQUAL:
            return E_BOOL;
        default:
            return E_UNKNOWN;
    }
}
static inline int get_e_type(int symb)
{
    switch(symb) {
        case E_BOOL:
        case E_NUM:
        case E_STR:
        case E_NIL:
            return symb;
        default:
            return E_UNKNOWN;
    }
}

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
const int prec_table[][E_MARK] = { // ma sirku poctu symbolu, posledni je znacka <
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
    int e_type;
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

// Smazani zasobniku
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
int s_push(Stack *stack, int type, int e_type)
{
    Node *new = malloc(sizeof(Node));
    if(new == NULL)
        return ERROR_GEN_MEM;
    new->type = type;
    new->e_type = e_type;
    new->next  = stack->top;
    stack->top = new;
    if(is_terminal(type))
        stack->active = new;
    return 1;
}

// Odstraneni prvku ze zasobniku
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
        return ERROR_GEN_MEM;

    new->type = stack->active->type;
    new->e_type = stack->active->e_type;
    new->next = stack->active->next;
    stack->active->next = new;
 //   if(stack->active == stack->top){
 //       stack->top = new;
 //   }
    stack->active->type = E_MARK;
    //na chvilu se rozbije active
    return 1;
}


#ifdef DEBUG
//jen pro debug
void s_dump(Stack *stack)
{
        Stack s_tmp;
        s_init(&s_tmp);
        char * type[] = {"^","*","/","+","-","..","<",">","<=",">=","~=","==",
                         "i","n","s","b","nil","(",")", ",","$", "{","E","P"};
        Node * tmp = stack->top;
        while(tmp!=NULL){
            s_push(&s_tmp, tmp->type, 0);
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
#endif

//ted uz je asi zbytecny aby to byla zvlast fce...
static inline void * __generate(int E)
{
    const int conv_inst[] = {
        [E_IDENT] = IPUSHI,
        [E_NUM] = IPUSH,
        [E_STR] = IPUSH,
        [E_BOOL] = IPUSH,
        [E_NIL] = IPUSH,
        [E_POW] = IPOW,
        [E_MUL] = IMUL,
        [E_DIV] = IDIV,
        [E_PLUS] = IADD,
        [E_MINUS] = ISUB,
        [E_STRCONCAT] = ICONCAT,
        [E_LESS] = ICMPL,
        [E_GREAT] = ICMPG,
        [E_LESSEQ] = ICMPEL,
        [E_GREATEQ] = ICMPEG,
        [E_NOTEQ] = ICMP,
        [E_EQUAL] = ICMPN
    };
    //adresy!!
    return (void *) generate(conv_inst[E], NULL, E);
}

// Aplikace pravidla (reakce na >)
static int s_oobely_boo(Stack *stack)
{
    enum states { START, VAL, VAR, RBRAC, EEEE, EEEE_COMM, BRAC_EEEE, FUNC_N, FUNC_CALL, OPER};
    const int translate[] = {
        [E_POW] = E_OP,
        [E_MUL] = E_OP,
        [E_DIV] = E_OP,
        [E_PLUS] = E_OP,
        [E_MINUS] = E_OP,
        [E_STRCONCAT] = E_OP,
        [E_LESS] = E_OP,
        [E_GREAT] = E_OP,
        [E_LESSEQ] = E_OP,
        [E_GREATEQ] = E_OP,
        [E_NOTEQ] = E_OP,
        [E_EQUAL] = E_OP,
        [E_IDENT] = E_IDENT,
        [E_NUM] = E_LIT,
        [E_STR] = E_LIT,
        [E_BOOL] = E_LIT,
        [E_NIL] = E_LIT,
        [E_LBRAC] = E_LBRAC,
        [E_RBRAC] = E_RBRAC,
        [E_COMMA] = E_COMMA,
        [E_DOLLAR] = E_DOLLAR,
        [E_MARK] = E_MARK,
        [E_NET_E] = E_NET_E,
        [E_NET_P] = E_NET_P,
    };

    int E1, E2, OP; // pro uchovani typu redukovanych E

    int top;
    int state = START;
    // cyklus prochazeni zasobnikem
    for(E1 = stack->top->e_type; ;s_pop(stack)) {
        top = translate[stack->top->type];
        switch(state) {
            case START: // startovaci stav
                if(top == E_LIT)
                    state = VAL;
                else if(top == E_IDENT)
                    state = VAR;
                else if(top == E_RBRAC)
                    state = RBRAC;
                else if(top == E_NET_E)
                    state = EEEE;
                else
                    return ERROR_SYN_EXP_FAIL;
                break;
            case VAR: // E -> id
                func_pop(); // neslo o funkci, pop
            case VAL: // E -> string, bool, num, nil
                // na zasobniku je hodnota, ocekava se konec a redukce
                if(top != E_MARK)
                    return ERROR_SYN_EXP_FAIL;
                s_pop(stack); // oddelani znacky
                try( s_push(stack, E_NET_E, E1) );
#ifdef DEBUG
    printf("I: PUSH(I) %p %d NULL\n", NULL, E1);
#endif
                __generate(E1);
                return 1;
                break;
            case RBRAC: // na zasobniku je ... )
                if(top == E_NET_E) {
                    E1 = stack->top->e_type;
                    state = BRAC_EEEE;
                }
                else if(top == E_LBRAC)
                    state = FUNC_CALL;
                else if(top == E_NET_P)
                    state = FUNC_N;
                else
                    return ERROR_SYN_EXP_FAIL;
                break;
            case EEEE: // na zasobniku je ... E
                if(top == E_OP) {
                    OP = stack->top->type;
                    state = OPER;
                }
                else if(top == E_COMMA)
                    state = EEEE_COMM;
                else
                    return ERROR_SYN_EXP_FAIL;
                break;
            case BRAC_EEEE: // na zasobniku je ... E )
                if(top != E_LBRAC)
                    return ERROR_SYN_EXP_FAIL;
                s_pop(stack);
                top = translate[stack->top->type];
                if(top == E_IDENT){ // volani funkce s 1 parametrem
                    s_pop(stack); // oddelani identifikatoru
                    top = translate[stack->top->type];
                    // asi bylo volani funkce a ja vim ze mela 1 parametr
                    func_inc();
#ifdef DEBUG
    printf("Byla volana funkce s %d parametry\n", func_stack[F]);
#endif
                    func_pop();
                    E1 = E_UNKNOWN;
                }
                // bud vyraz v zacorce, nebo volani funkce
                if(top == E_MARK) {
                    s_pop(stack); // oddelani znacky
                    try( s_push(stack, E_NET_E, E1) );
                    return 1;
                }
                else
                    return ERROR_SYN_EXP_FAIL;
                break;
            case FUNC_CALL: // na zasobniku je () nebo ( P )
                // overeni, ze tam je identifikator a znacka
                if(top != E_IDENT)
                    return ERROR_SYN_EXP_FAIL;
                s_pop(stack);
                top = translate[stack->top->type];
                if(top != E_MARK)
                    return ERROR_SYN_EXP_FAIL;
                s_pop(stack); // oddelani znacky
                try( s_push(stack, E_NET_E, E_UNKNOWN) );
                // bylo volani funkce a ja vim, kolik mela parametru
#ifdef DEBUG
    printf("Byla volana funkce s %d parametry\n", func_stack[F]);
#endif
                func_pop();
                return 1;
                break;
            case FUNC_N: // na zasobniku je ...  P )
                if(top == E_LBRAC)
                    state = FUNC_CALL;
                else
                    return ERROR_SYN_EXP_FAIL;
                break;
            case OPER: // na zasobniku je ... E op E
                if(top != E_NET_E)
                    return ERROR_SYN_EXP_FAIL;
                E2 = stack->top->e_type;
                s_pop(stack);
                top = translate[stack->top->type];
                if(top != E_MARK)
                    return ERROR_SYN_EXP_FAIL;
                s_pop(stack); // oddelani znacky
                if(E1 != E2 && E1 != E_UNKNOWN && E2 != E_UNKNOWN)
                    return ERROR_SEM_WRONG_TYPES;
                try( s_push(stack, E_NET_E, get_result_type(OP)) );
#ifdef DEBUG
    printf("I: (OPER)%d NULL NULL NULL\n", OP);
#endif
                __generate(OP);
                return 1;
                break;
            case EEEE_COMM: // na zasobniku je ... , E
                if(top != E_NET_E && top != E_NET_P)
                    return ERROR_SYN_EXP_FAIL;
                if(top == E_NET_E) // pokud je to E, tak se musi zapocitat o param navic
                    func_inc();
                s_pop(stack);
                top = translate[stack->top->type];
                if(top == E_MARK) {
                    s_pop(stack); // oddelani znacky
                    try( s_push(stack, E_NET_P, E_UNKNOWN) );
                    // byl parametr, musim pricist
                    func_inc();
                    return 1;
                }
                else
                    return ERROR_SYN_EXP_FAIL;
                break;
        }
    }

    // TODO v cyklu chybi generovani instrukci
    // nejak se musi jeste pocitat parametry funci a pripadne je doplnit nil
    // je to napsane celkem prasacky, asi budu muset nekde uvest, jak to funguje
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static inline int expression__(Stack *stack)
{
    int a, b;

    // radsi kontrola, on to nikdo predtim asi nedela a to tabulky s tim nemuzu
    if(token < 0)
        return token;

    // Takhle to zacina, da se dolar na zasobnik
    try( s_push(stack, E_DOLLAR, E_UNKNOWN) );

    a = translatetoken[token]; // aktualni vstup
    do {
        b = stack->active->type; // nejvrchnejsi terminal na zasobniku

        // pokud je identifikator, da se predpokladat, ze to bude funkce
        if(a == E_IDENT)
            func_push();

        switch( prec_table[b][a] ) {
            case EQ:
                // push(a)
                try( s_push(stack, a, E_UNKNOWN) );
                b = stack->active->type; // doslo ke zmene horniho terminalu
                // TODO vygenerovat instrukci, pokud je potreba
                  // tady tohle se stane jen pro id ( a ( ), toho se mozna
                  // bude dat vyuzit nejak
                // precist novy token
                get_token();
                if (token < 0) // nacetl jsem neco spatneho
                    return token;
                a = translatetoken[token];
                break;

            case LT:
                // b na zasobniku vymenit za b<
                try( s_alter(stack) );
                // push(a)
                try( s_push(stack, a, get_e_type(a)) );
                // pokud je carka mimo funkci, tak je konec vyrazu (kvuli write)
                if(a == E_COMMA && F == -1) {
                    a = E_DOLLAR;
                    continue;
                }
                // TODO vygenerovat instrukci
                  // push nil, true, false
                  // nebo pridat literal do tabulky a push s odkazem na literal
                  // nebo push a odkaz na promennou
                b = stack->active->type; // doslo ke zmene horniho terminalu
                // precist novy token
                get_token();
                if (token < 0) // nacetl jsem neco spatneho
                    return token;
                a = translatetoken[token];
                break;

            case GT:
                // pokud je na zasobiku <y a existuje pravidlo r: A -> y, pak
                // vymenit <y za A, a pouzit to pravidlo
                try( s_oobely_boo(stack) );
                b = stack->active->type; // nejhornejsi terminal byl zmenen
                break;

            case OO:
            default:
                // pokud je neocekavanym tokenem prava zavorka,
                // tak se rekne, ze je konec vyrazu, kvuli konci write( ... )
                if(token == RBRAC) {
                    a = E_DOLLAR;
                    continue;
                }
                // jinak syntakticka chyba
                return ERROR_SYN_EXP_FAIL;
                break;
        } /* switch */
#ifdef DEBUG
        s_dump(stack);
#endif
    } while(a != E_DOLLAR || b != E_DOLLAR);

    return 1;
}


/*
 * Wrapper pro funkci na zpracovani vyrazu
 */
int expression(void)
{
    Stack stack;
    // inicializace zasobniku
    s_init(&stack);
    // samotne zpracovani vyrazu
    int x = expression__(&stack);
    // uvolneni zasobniku
    s_clean(&stack);
    return x;
}
