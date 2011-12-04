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
    IRETP,      // 5  navrat z funkce, pop az na zarazku, navratovou hodnotu ziska z vrcholu zasobniku
    IRET,       // 6  navrat z funkce, pop az na zarazku, navratova hodnota nil
    IPUSH,      // 7  na vrchol zasobniku pushne literal na zadane adrese
    IPUSHI,     // 8  na vrchol zasobniku pushne lokalni promennou/argument na zadane adrese
    IPUSHT,     // 9  na vrchol zasobniku pushne true
    IPUSHF,     // 10 na vrchol zasobniku pushne false
    IPOPI,      // 11 popne hodnotu z vrcholu zasobniku do lokalni promenne/argumentu
    IPUSHN,     // 12 pushne do zasobniku nil
    IADD,       // 13 popne vrchni dve hodnoty, secte je, vysledek pushne
    ISUB,       // 14 popne vrchni dve hodnoty, odecte, vysledek pushne
    IMUL,       // 15 vynasobi vrchni dve hodnoty na zasobniku, vysledek pushne
    IDIV,       // 16 vydeli vrchni dve hodnoty na zasobniku, vysledek pushne
    IPOW,       // 17 umocni prvni na druhou hodnotu na zasobniku, vysledek pushne
    ICONCAT,    // 18 spoji vrchni dva stringy v zasobniku, vysledek pushne
    ICMP,       // 19 porovna vrchni dve hodnoty na zasobniku, vysledek pushne
    ICMPN,      // 20 porovna na nerovnost
    ICMPL,      // 21 porovna na mensi nez
    ICMPG,      // 22 porovna na vetsi nez
    ICMPEL,     // 23 porovna na mensi nebo rovno
    ICMPEG,     // 24 porovna na vetsi nebo rovno
    IWRITE,     // 25 prehaze argumenty az po zarazku do druheho zasobniku, pak je postupne vypise
    IREAD,      // 26 prebere "nejspis" dva argumenty, podle prvniho fmt retezce nacte hodnotu na adresu druheho
    ITYPE,      // 27 popne ze zasobniku nejvyssi hodnotu, zjisti jeji typ, ktery pote pushne na zasobnik
    ISUBSTR,    // 28 popne vrchni tri argumenty, string a dve cisla, vytvori podle toho podretezec, vysledek pushne
    IFIND,      // 29 popne vrchni dva stringy, vyhleda druhy v prvnim, pushne index
    ISORT,      // 30 popne vrchni string, setridi ho a vysledek pushne
    INOP,       // 31 neudela nic, pouzivano jako label
} Instructions;

enum {
    ANONE, // zadna adresa, bude se pouzivat vetsinou
    ALITTABLE, // typ ukazatele do tabulky literalu
    ALOCTABLE, // typ ukazatel do lokalni tabulky symbolu - lokalni promenne, argumenty
    AFUNCTABLE, // ukazatel do tabulky funkci
    ATAPE, // ukazatel na pasku, pouzivane u skoku
    //TODO: vymyslet, co vsechno tady ma byt
} AdrTypes;

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

#ifdef DEBUG
int print_tape();
#endif

PTapeItem generate(int, void *, int); /// funkce, ktera prebere typ instrukce, ukazatel a jeho typ a vygeneruje podle toho instrukci

int execute(); /// funkce, ktera vezme instrukce z globalni tabulky prvku a vykona je

// globalni paska instrukci
extern Tape tape;

#endif 
