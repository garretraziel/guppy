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

//globalni promenna, paska pro ulozeni instrukci
Tape tape;

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
    return (tape.act = tape.act -> next);
}

int generate(int instr, void *adr, int type) /// funkce, ktera prebere typ instrukce, ukazatel a jeho typ a vygeneruje podle toho instrukci
{
    //TODO: takto jednoduse to samozrejme nejde, ale aspon aby byl mustr. joa funkce by mohla vracet adresu te vygenerovane instrukce
    //TODO: jinak se asi bude muset switchovat podle typu instrukce a vytvaret vsechny ty dodatecny veci, co jsou k tomu potreba
    PTapeItem item = malloc(sizeof(struct TTapeItem));
    if (item == NULL) return -1; //TODO: potreba udelat errory pro nedostatek pameti
    item -> instr = instr;
    item -> adr = adr;
    item -> adrtype = type;
    item -> next = NULL;
    if (add_to_tape(item) != 0) return -1;
    
    return 0;
}

int execute() /// funkce, ktera vezme instrukce z globalni tabulky prvku a vykona je
{
    //TODO: exekuce
    return 0;
}
