/*
 * projekt: interpret jazyka IFJ11 
 * soubor: exec.h
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#ifndef GUPPY_EXEC_H
#define GUPPY_EXEC_H

enum {
    IHALT,      // 0  zastav vykonavani programu
    IJMP,       // 1  skoc na adresu z operandu
    IJMPT,      // 2  skoc na adresu z operandu, je-li na vrcholu zasobniku true, pop
    IJMPF,      // 3  skoc na adresu z operandu, je-li na vrcholu zasobniku false, pop
    ICALL,      // 4  zavolej funkci na adrese ve stromu funkci
    IRET,       // 5  navrat z funkce, pop az na zarazku, navratovou hodnotu ziska z vrcholu zasobniku POZOR: pokud nema navratovou hodnotu, musi se pred retp dat pushn
    IPUSH,      // 6  na vrchol zasobniku pushne literal na zadane adrese
    IPUSHI,     // 7  na vrchol zasobniku pushne lokalni promennou/argument na zadane adrese
    IPUSHT,     // 8  na vrchol zasobniku pushne true
    IPUSHF,     // 9  na vrchol zasobniku pushne false
    IPOPI,      // 10 popne hodnotu z vrcholu zasobniku do lokalni promenne/argumentu
    IPUSHN,     // 11 pushne do zasobniku nil
    IADD,       // 12 popne vrchni dve hodnoty, secte je, vysledek pushne
    ISUB,       // 13 popne vrchni dve hodnoty, odecte, vysledek pushne
    IMUL,       // 14 vynasobi vrchni dve hodnoty na zasobniku, vysledek pushne
    IDIV,       // 15 vydeli vrchni dve hodnoty na zasobniku, vysledek pushne
    IPOW,       // 16 umocni prvni na druhou hodnotu na zasobniku, vysledek pushne
    ICONCAT,    // 17 spoji vrchni dva stringy v zasobniku, vysledek pushne
    ICMP,       // 18 porovna vrchni dve hodnoty na zasobniku, vysledek pushne
    ICMPN,      // 19 porovna na nerovnost
    ICMPL,      // 20 porovna na mensi nez
    ICMPG,      // 21 porovna na vetsi nez
    ICMPEL,     // 22 porovna na mensi nebo rovno
    ICMPEG,     // 23 porovna na vetsi nebo rovno
    IWRITE,     // 24 prehaze argumenty az po zarazku do druheho zasobniku, pak je postupne vypise
    IREAD,      // 25 prebere "nejspis" dva argumenty, podle prvniho fmt retezce nacte hodnotu na adresu druheho
    ITYPE,      // 26 popne ze zasobniku nejvyssi hodnotu, zjisti jeji typ, ktery pote pushne na zasobnik
    ISUBSTR,    // 27 popne vrchni tri argumenty, string a dve cisla, vytvori podle toho podretezec, vysledek pushne
    IFIND,      // 28 popne vrchni dva stringy, vyhleda druhy v prvnim, pushne index
    ISORT,      // 29 popne vrchni string, setridi ho a vysledek pushne
    INOP,       // 30 neudela nic, pouzivano jako label
} Instructions;

enum {
    DNUM, // datovy typ cislo
    DSTRING, // datovy typ string
    DBOOL, // datovy typ boolean
    DNIL, // datovy typ nil
    DREGISTER, // ulozeni EBP/ESP
    DRETADR, // navratova adresa z funkce
} StackDatTypes; // seznam datovych typu, ktere mohou byt ulozene na zasobniku

typedef struct TTapeItem { // paska pro ulozeni instrukci
    int instr; // samotna instrukce podle enum Instructions
    void *adr; // adresa, ktera se treba pouzije
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

#ifdef DEBUG
int print_tape();
#endif

PTapeItem generate(int, void *); /// funkce, ktera prebere typ instrukce a ukazatel a vygeneruje podle toho instrukci

int execute(); /// funkce, ktera vezme instrukce z globalni tabulky prvku a vykona je

// globalni paska instrukci
extern Tape tape;
extern PTapeItem main_pointer;

#endif 
