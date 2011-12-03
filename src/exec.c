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

//globalni promenne, paska pro ulozeni instrukci a zasobnik
Tape tape;
Data **stack;

int init_stack(int size);
int pop_stack(int *dattype, univalue *value);
int push_stack(int dattype, univalue value);
int delete_stack();

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
