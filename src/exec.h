/*
 * projekt: interpret jazyka IFJ11 
 * soubor: exec.h
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#ifndef IFJ_EXEC_H
#define IFJ_EXEC_H

enum {
    IHALT,      // zastav vykonavani programu
    IJMP,       // skoc na adresu z operandu
    IJMPT,      // skoc na adresu z operandu, je-li na vrcholu zasobniku true, pop
    IJMPF,      // skoc na adresu z operandu, je-li na vrcholu zasobniku false, pop
    ICALL,      // zavolej funkci na adrese ve stromu funkci
    IRETP,      // navrat z funkce, pop az na zarazku, navratovou hodnotu ziska z vrcholu zasobniku
    IRET,       // navrat z funkce, pop az na zarazku, navratova hodnota nil
    IPUSH,      // na vrchol zasobniku pushne literal na zadane adrese
    IPUSHI,     // na vrchol zasobniku pushne lokalni promennou/argument na zadane adrese
    IPUSHT,     // na vrchol zasobniku pushne true
    IPUSHF,     // na vrchol zasobniku pushne false
    IPOPI,      // popne hodnotu z vrcholu zasobniku do lokalni promenne/argumentu
    IPUSHM,     // pushne zarazku pro uklizeni po funkci
    IADD,       // popne vrchni dve hodnoty, secte je, vysledek pushne
    ISUB,       // popne vrchni dve hodnoty, odecte, vysledek pushne
    IMUL,       // vynasobi vrchni dve hodnoty na zasobniku, vysledek pushne
    IDIV,       // vydeli vrchni dve hodnoty na zasobniku, vysledek pushne
    IPOW,       // umocni prvni na druhou hodnotu na zasobniku, vysledek pushne
    ICONCAT,    // spoji vrchni dva stringy v zasobniku, vysledek pushne
    ICMP,       // porovna vrchni dve hodnoty na zasobniku, vysledek pushne
    ICMPN,      // porovna na nerovnost
    ICMPL,      // porovna na mensi nez
    ICMPG,      // porovna na vetsi nez
    ICMPEL,     // porovna na mensi nebo rovno
    ICMPEG      // porovna na vetsi nebo rovno
} Instructions;

enum {
    ALITTABLE, // typ ukazatele do tabulky literalu
    ALOCTABLE, // typ ukazatel do lokalni tabulky symbolu - lokalni promenne, argumenty
    AFUNCTABLE, // ukazatel do tabulky funkci
    //TODO: vymyslet, co vsechno tady ma byt
} AdrTypes;

typedef struct TTapeItem { // paska pro ulozeni instrukci
    int instr; // samotna instrukce podle enum Instructions
    void *adr; // adresa, ktera se treba pouzije
    int adrtype; // typ ukazatele
    struct TTapeItem *next; // ukazatel na dalsi instrukci
} *PTapeItem;

typedef struct TTape {
    PTapeItem top; // vrchol listu
    PTapeItem bot; // spodek listu
    PTapeItem act; // aktivni prvek listu
} Tape;

// POZOR! paska bude globalni
void init_tape(); /// inicializuje pasku s kodem
int add_to_tape(PTapeItem); /// prida do pasky s kodem
int delete_tape(); /// smaze pasku
PTapeItem actnext_tape(); /// posune aktivni prvek na dalsi a vrati ho
//TODO: nebude jich potreba vic?...

int generate(int, void *, int); /// funkce, ktera prebere typ instrukce, ukazatel a jeho typ a vygeneruje podle toho instrukci

int execute(); /// funkce, ktera vezme instrukce z globalni tabulky prvku a vykona je

#endif 