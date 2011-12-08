/*
 * projekt: interpret jazyka IFJ11 
 * soubor: expr.c
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "string.h"
#include "expr.h"
#include "defines.h"
#include "lexical.h"
#include "exec.h"
#include "ial.h"


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

// 1) == a != muze mit libovolne operandy
// 2) pro ostatni operatory musi byt stejneho typu stejneho typu
// nebo alespon jeden z nich unknown nebo promenna
// jinak je to blbe
// 3) kontrola, zda typ operandu je ok pro dany operand
// < > <= >= jen pro number a string
// .. jen string
// ^ + - * / jen number
#define check_operands_and_operator() do { int a, b;\
    a = (E1==E_UNKNOWN || E1==E_VAR)?E_UNKNOWN:E1;  \
    b = (E2==E_UNKNOWN || E2==E_VAR)?E_UNKNOWN:E2;  \
    if(OP == E_EQUAL || OP == E_NOTEQ) \
        break; \
    if(a != b && a != E_UNKNOWN && b != E_UNKNOWN)  \
        return ERROR_SEM_WRONG_TYPES; \
    switch(OP) { \
        case E_LESS: case E_LESSEQ: case E_GREAT: case E_GREATEQ: \
            if(a != E_UNKNOWN && a != E_NUM && a != E_STR && \
               b != E_UNKNOWN && b != E_NUM && b != E_STR) \
                return ERROR_SEM_WRONG_TYPES; \
            break; \
        case E_STRCONCAT: \
            if(a != E_UNKNOWN && a != E_STR && \
               b != E_UNKNOWN && b != E_STR) \
                return ERROR_SEM_WRONG_TYPES; \
            break; \
        default: \
            if(a != E_UNKNOWN && a != E_NUM && \
               b != E_UNKNOWN && b != E_NUM) \
                return ERROR_SEM_WRONG_TYPES; \
    } \
    } while(0)


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
/* ^    */ { LT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, LT, LT, LT, LT, LT, LT, GT, GT, GT, },
/* *    */ { LT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, LT, LT, LT, LT, LT, LT, GT, GT, GT, },
/* /    */ { LT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, LT, LT, LT, LT, LT, LT, GT, GT, GT, },
/* +    */ { LT, LT, LT, GT, GT, GT, GT, GT, GT, GT, GT, GT, LT, LT, LT, LT, LT, LT, GT, GT, GT, },
/* -    */ { LT, LT, LT, GT, GT, GT, GT, GT, GT, GT, GT, GT, LT, LT, LT, LT, LT, LT, GT, GT, GT, },
/* ..   */ { LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, GT, GT, LT, LT, LT, LT, LT, LT, GT, GT, GT, },
/* <    */ { LT, LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, GT, LT, LT, LT, LT, LT, LT, GT, GT, GT, },
/* >    */ { LT, LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, GT, LT, LT, LT, LT, LT, LT, GT, GT, GT, },
/* <=   */ { LT, LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, GT, LT, LT, LT, LT, LT, LT, GT, GT, GT, },
/* >=   */ { LT, LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, GT, LT, LT, LT, LT, LT, LT, GT, GT, GT, },
/* ~=   */ { LT, LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, GT, LT, LT, LT, LT, LT, LT, GT, GT, GT, },
/* ==   */ { LT, LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, GT, LT, LT, LT, LT, LT, LT, GT, GT, GT, },
/* id   */ { GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, OO, OO, OO, OO, OO, EQ, GT, GT, GT, },
/* num  */ { GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, OO, OO, OO, OO, OO, OO, GT, GT, GT, },
/* str  */ { GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, OO, OO, OO, OO, OO, OO, GT, GT, GT, },
/* bool */ { GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, OO, OO, OO, OO, OO, OO, GT, GT, GT, },
/* nil  */ { GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, OO, OO, OO, OO, OO, OO, GT, GT, GT, },
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
    void *ptr;
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
int s_push(Stack *stack, int type, int e_type, void *ptr)
{
//    printf("s_push, %d, %d, %p\n", type, e_type, ptr);
    Node *new = malloc(sizeof(Node));
    if(new == NULL)
        return ERROR_GEN_MEM;
    new->type = type;
    new->e_type = e_type;
    new->ptr = ptr;
    new->next  = stack->top;
    stack->top = new;
    if(is_terminal(type))
        stack->active = new;
    return 1;
}

// Odstraneni prvku ze zasobniku
void s_pop(Stack *stack)
{
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
            s_push(&s_tmp, tmp->type, 0, NULL);
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


// Aplikace pravidla (reakce na >)
static int s_oobely_boo(Stack *stack)
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
        [E_NOTEQ] = ICMPN,
        [E_EQUAL] = ICMP,
    };
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

    int E1, E2, OP = 0; // pro uchovani typu redukovanych E
    int top;
    int state = START;
    // cyklus prochazeni zasobnikem
    for(E1 = stack->top->e_type; ;s_pop(stack)) {
        top = translate[stack->top->type];
        switch(state) {
            case START: // startovaci stav
                if(top == E_LIT)
                    state = VAL;
                else if(top == E_IDENT) {
                    // kontrola, ze to neni funkce
                    if(stack->top->e_type == E_FUNC)
                        return ERROR_SEM_FUNC_VAR;
                    state = VAR;
                } else if(top == E_RBRAC)
                    state = RBRAC;
                else if(top == E_NET_E)
                    state = EEEE;
                else
                    return ERROR_SYN_EXP_FAIL;
                break;
            case VAR: // E -> id
            case VAL: // E -> string, bool, num, nil
                // na zasobniku je hodnota, ocekava se konec a redukce
                if(top != E_MARK)
                    return ERROR_SYN_EXP_FAIL;
                s_pop(stack); // oddelani znacky
                try( s_push(stack, E_NET_E, E1, NULL) );
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
                    // overeni poctu parametru
                    for(int i = 1; i < ((FunctionTree*)stack->top->ptr)->params; ++i)
                        generate(IPUSHN, NULL);
                    for(int i = 1; i > ((FunctionTree*)stack->top->ptr)->params; --i)
                        generate(IPOP, NULL);
                    // instrukce call podle toho, co to je za funkce
                    if(stack->top->e_type == E_FUNC)
                        generate(ICALL, stack->top->ptr);
                    else if(stack->top->e_type == E_FIND)
                        generate(IFIND, stack->top->ptr);
                    else if(stack->top->e_type == E_SORT)
                        generate(ISORT, stack->top->ptr);
                    else if(stack->top->e_type == E_SUBSTR)
                        generate(ISUBSTR, stack->top->ptr);
                    else // TYPE
                        generate(ITYPE, stack->top->ptr);
                    s_pop(stack); // oddelani identifikatoru
                    top = translate[stack->top->type];
                    func_pop();
                    E1 = E_UNKNOWN;
                }
                // bud vyraz v zavorce, nebo volani funkce
                if(top == E_MARK) {
                    s_pop(stack); // oddelani znacky
                    try( s_push(stack, E_NET_E, E1, NULL) );
                    return 1;
                }
                else
                    return ERROR_SYN_EXP_FAIL;
                break;
            case FUNC_CALL: // na zasobniku je () nebo ( P )
                // overeni, ze tam je identifikator a znacka
                if(top != E_IDENT)
                    return ERROR_SYN_EXP_FAIL;
                // identifikator musi byt funkce
                if(stack->top->e_type < E_FUNC)
                    return ERROR_SEM_CALL_VAR;
                // overeni poctu parametru
                for(int i = func_stack[F]; i < ((FunctionTree*)stack->top->ptr)->params; ++i)
                    generate(IPUSHN, NULL);
                for(int i = func_stack[F]; i > ((FunctionTree*)stack->top->ptr)->params; --i)
                    generate(IPOP, NULL);
                // instrukce call podle toho, co to je za funkce
                if(stack->top->e_type == E_FUNC)
                    generate(ICALL, stack->top->ptr);
                else if(stack->top->e_type == E_FIND)
                    generate(IFIND, stack->top->ptr);
                else if(stack->top->e_type == E_SORT)
                    generate(ISORT, stack->top->ptr);
                else if(stack->top->e_type == E_SUBSTR)
                    generate(ISUBSTR, stack->top->ptr);
                else // TYPE
                    generate(ITYPE, stack->top->ptr);
                s_pop(stack);
                top = translate[stack->top->type];
                if(top != E_MARK)
                    return ERROR_SYN_EXP_FAIL;
                s_pop(stack); // oddelani znacky
                try( s_push(stack, E_NET_E, E_UNKNOWN, NULL) );
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
                // kontrola, zda jsou operandy stejneho nebo neznameho typu
                check_operands_and_operator();
                // kontrola, ze jsou operandy pouzitelne pro dany operator
                // TODO
                try( s_push(stack, E_NET_E, get_result_type(OP), NULL) );
                generate(conv_inst[OP], NULL);
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
                    try( s_push(stack, E_NET_P, E_UNKNOWN, NULL) );
                    // byl parametr, musim pricist
                    func_inc();
                    return 1;
                }
                else
                    return ERROR_SYN_EXP_FAIL;
                break;
        } /* switch */
    } /* for */

    // TODO v cyklu chybi generovani instrukci
    // nejak se musi jeste pocitat parametry funci a pripadne je doplnit nil
    // je to napsane celkem prasacky, asi budu muset nekde uvest, jak to funguje
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static inline int expression__(Stack *stack)
{
    int a, b;
    void *ptr = NULL;
    Data data;

    // radsi kontrola, on to nikdo predtim asi nedela a to tabulky s tim nemuzu
    if(token < 0)
        return token;

    // Takhle to zacina, da se dolar na zasobnik
    try( s_push(stack, E_DOLLAR, E_UNKNOWN, NULL) );

    a = translatetoken[token]; // aktualni vstup
    do {
        b = stack->active->type; // nejvrchnejsi terminal na zasobniku

        switch( prec_table[b][a] ) {
            case EQ:
                // push(a)
                try( s_push(stack, a, E_UNKNOWN, NULL) );
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
                // pokud je carka mimo funkci, tak je konec vyrazu (kvuli write)
                if(a == E_COMMA && F == -1) {
                    a = E_DOLLAR;
                    continue;
                }
                // TODO vygenerovat instrukci
                switch(a) {
                    // pokud je to identifikator, tak ho najdu v tabulce
                    case E_IDENT:
                        if((ptr = find_function(str.str))) {
                            func_push(); // nova funkce
                            // kontrola, co to je za funkci
                            if(strcmp("find", str.str) == 0)
                                try( s_push(stack, a, E_FIND, ptr) );
                            else if(strcmp("sort", str.str) == 0)
                                try( s_push(stack, a, E_SORT, ptr) );
                            else if(strcmp("substr", str.str) == 0)
                                try( s_push(stack, a, E_SUBSTR, ptr) );
                            else if(strcmp("type", str.str) == 0)
                                try( s_push(stack, a, E_TYPE, ptr) );
                            else 
                                try( s_push(stack, a, E_FUNC, ptr) );
                        } else if((ptr = find_local(str.str))) {
                            generate(IPUSHI, ptr); 
                            try( s_push(stack, a, E_VAR, ptr) );
                        } else
                            return ERROR_SEM_VAR_UND;
                        break;
                    // pokud hodnota tak push
                    case E_NIL:
                        generate(IPUSHN, NULL);
                        try( s_push(stack, a, E_NIL, NULL) );
                        break;
                    case E_BOOL:
                        generate((token == TRUE)? IPUSHT:IPUSHF, NULL); 
                        try( s_push(stack, a, E_BOOL, NULL) );
                        break;
                    case E_NUM:
                        try( s_push(stack, a, E_NUM, NULL) );
                        data.type = T_NUMBER;
                        data.value.num = strtod(str.str, NULL);
                        // vegenerovani push
                        try( insert_literal(data) );
                        generate(IPUSH, last_literal);
                        break;                    
                    case E_STR:
                        try( s_push(stack, a, E_STR, NULL) );
                        data.type = T_STRING;
                        data.value.str = str.str;
                        try( insert_literal(data) );
                        try( str_new(&str, STR_INIT_LEN) );
                        // vegenerovani push
                        generate(IPUSH, last_literal); 
                        break;
                    // ostatni nic mimoradneho nedelaji
                    default:
                        try( s_push(stack, a, get_e_type(a), NULL) );
                        break;
                }
                // push(a)
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

    return stack->top->e_type;
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
