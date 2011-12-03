/*
 * projekt: interpret jazyka IFJ11 
 * soubor: exec.c
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#include <stdlib.h>

#include "exec.h"
#include "string.h"
#include "ial.h"

typedef struct TStack {
    int esp;
    int size;
    Data **val;
} *PStack;

//globalni promenne, paska pro ulozeni instrukci a zasobnik
Tape tape;
PStack stack;

int init_stack(int size); /// inicializuje zasobnik
int pop_stack(int *dattype, univalue *value); /// popne ze zasobniku vrchni hodnotu, vrati take jeji typ
int push_stack(int dattype, univalue value); /// pushne na zasobnik hodnotu i jeji datovy typ
int delete_stack(); /// smaze cely zasobnik

void init_tape() /// inicializuje pasku s kodem
{
    tape.act = tape.bot = tape.top = NULL;
}

int add_to_tape(PTapeItem item) /// prida do pasky s kodem
{
    if (tape.bot == NULL) {
        tape.bot = item;
        tape.top = item;
    } else {
        tape.bot -> next = item;
        tape.bot = item;
    }
    
    return 0;
}

int delete_tape() /// smaze pasku
{
    for (PTapeItem item = tape.top; item != NULL; item = item -> next) {
        PTapeItem temp = item;
        free(temp);
    }

    return 0;
}

PTapeItem actnext_tape() /// posune aktivni prvek na dalsi a vrati ho
{
    if (tape.act == NULL) return NULL;
    PTapeItem temp =  tape.act;
    tape.act = tape.act -> next; // posunu se na dalsi instrukci
    return temp;
}

PTapeItem generate(int instr, void *adr, int type) /// funkce, ktera prebere typ instrukce, ukazatel a jeho typ a vygeneruje podle toho instrukci
{
    //TODO: takto jednoduse to samozrejme nejde, ale aspon aby byl mustr. joa funkce by mohla vracet adresu te vygenerovane instrukce
    //TODO: jinak se asi bude muset switchovat podle typu instrukce a vytvaret vsechny ty dodatecny veci, co jsou k tomu potreba
    PTapeItem item = malloc(sizeof(struct TTapeItem));
    if (item == NULL) return NULL;
    item -> instr = instr;
    item -> adr = adr;
    item -> adrtype = type;
    item -> next = NULL;
    if (add_to_tape(item) != 0) {
        free(item);
        return NULL;
    }
    
    return item;
}

int execute() /// funkce, ktera vezme instrukce z globalni tabulky prvku a vykona je
{
    //TODO: exekuce

    if (tape.top == NULL) return 0; //TODO: ma to vubec cenu osetrovat? tohle se stejne nikdy nestane, ne?

    tape.act = tape.top; //TODO: aktualni se asi nikdy jindy nastavovat nebude, popremyslet o tom
    
    while (1) { // reknete mi duvod, proc by to nemohl byt nekonecny cyklus
        
        PTapeItem instr = actnext_tape();

        if (instr == NULL) return -1; //TODO: neco se nepovedlo, popremyslet co by to mohlo byt za chybu

        switch (instr -> instr) {
        case IHALT:
            delete_tape();
            return 0;
            break; // sem by to nemelo dojit
        case IJMP:
            tape.act = (PTapeItem) instr -> adr;
            break;
        case IJMPT:
            break;
        case IJMPF:
            break;
        case ICALL:
            break;
        case IRETP:
            break;
        case IRET:
            break;
        case IPUSH:
            break;
        case IPUSHI:
            break;
        case IPUSHT:
            break;
        case IPUSHF:
            break;
        case IPOPI:
            break;
        case IPUSHM:
            break;
        case IADD:
            break;
        case ISUB:
            break;
        case IMUL:
            break;
        case IDIV:
            break;
        case IPOW:
            break;
        case ICONCAT:
            break;
        case ICMP:
            break;
        case ICMPN:
            break;
        case ICMPL:
            break;
        case ICMPG:
            break;
        case ICMPEL:
            break;
        case ICMPEG:
            break;
        case IWRITE:
            break;
        case IREAD:
            break;
        case ITYPE:
            break;
        case ISUBSTR:
            break;
        case IFIND:
            break;
        case ISORT:
            break;
        default:
            //TODO: nedefinovana instrukce, POMOC!
            return -1; //TODO: co to ma vracet za chybu?
        }
        
    }
    
    return 0;
}

int init_stack(int size) /// inicializuje zasobnik
{
    stack -> val = malloc(sizeof(Data)*size);
    if (stack -> val == NULL) return -1; //TODO: err_mem
    stack -> size = size;
    stack -> esp = -1;
    return 0;
}

int pop_stack(int *dattype, univalue *value) /// popne ze zasobniku vrchni hodnotu, vrati take jeji typ
{
    if (stack -> esp == -1) return -1; //TODO: definovat error
    (*dattype) = stack -> val[stack -> esp] -> type;
    (*value) = stack -> val[stack -> esp] -> value;
    (stack -> esp)--;
    
    return 0;
}

int push_stack(int dattype, univalue value) /// pushne na zasobnik hodnotu i jeji datovy typ
{
    Data *temp = malloc(sizeof(Data));
    if (temp == NULL) return -1; //TODO: err_mem
    temp -> type = dattype;
    temp -> value = value;
    (stack -> esp)++;

    stack -> val[stack -> esp] = temp;
    
    return 0;
}

int delete_stack() /// smaze cely zasobnik
{
    for (int i = 0; i<= stack->esp; i++) {
        free(stack -> val[i]);
    }
    free(stack -> val);
    stack -> esp = -1;
    stack -> val = NULL;
    return 0;
}

