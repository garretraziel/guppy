/*
 * projekt: interpret jazyka IFJ11 
 * soubor: exec.c
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

//TODO: u kazdeho erroru taky spravne uvolnovat pamet. TO bude jeste peklo

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "exec.h"
#include "defines.h"
#include "string.h"
#include "ial.h"

// uvolni pamet a vrati error
#define ExecError(errcode) do {                 \
        delete_stack(); delete_tape();          \
        return errcode;                         \
    } while(0)
// pokusi se pushnout na stack. pokud se nepovede, vola error
#define try_push_stack(type,val,errcode) do {   \
        if (push_stack(type, val) != 0)         \
            ExecError(errcode);                 \
    } while(0)
// pokusi se pushnout. pokud se nepovede tak uvolni a vola error
#define free_try_push_stack(type,val,errcode,todelete) do {             \
        if (push_stack(type, val) != 0)                                 \
        {free(todelete); ExecError(errcode);}                           \
    } while(0)
// pokusi se popnout ze stacku, kdyztak vola error
#define try_pop_stack(type,val) do {                            \
        if (pop_stack(&type, &val) != 0)                        \
            ExecError(ERROR_INT_EMPTY_STACK);                   \
    } while(0)
// makro na opakujici se kod v porovnavacich instrukcich. prebira operator < > <= a >=
#define compare(operator) do {                                          \
        univalue value1, value2, retvalue;                              \
        int dattype1, dattype2;                                         \
        try_pop_stack(dattype1, value1);                                \
        if (pop_stack(&dattype2, &value2) != 0) {                       \
            if (dattype1 == DSTRING) free(value1.str);                  \
            ExecError(ERROR_INT_EMPTY_STACK);                           \
        }                                                               \
        if (dattype1 != dattype2) {                                     \
            if (dattype1 == DSTRING) free(value1.str);                  \
            if (dattype2 == DSTRING) free(value2.str);                  \
            ExecError(ERROR_INT_INCOMP_TYPES);                          \
        } else if (dattype1 == DNUM) {                                  \
            if (value2.num operator value1.num)                         \
                retvalue.log = STRUE;                                   \
            else                                                        \
                retvalue.log = SFALSE;                                  \
        } else if (dattype1 == DBOOL) {                                 \
            ExecError(ERROR_INT_BADPARAM);                              \
        } else if (dattype1 == DSTRING) {                               \
            if (strcmp(value2.str, value1.str) operator 0)              \
                retvalue.log = STRUE;                                   \
            else                                                        \
                retvalue.log = SFALSE;                                  \
            free(value1.str);                                           \
            free(value2.str);                                           \
        } else if (dattype1 == DNIL) ExecError(ERROR_INT_BADPARAM);     \
        try_push_stack(DBOOL, retvalue, ERROR_GEN_MEM);                 \
    } while(0)

// struktura reprezentujici zasobnik
typedef struct TStack {
    int esp;
    int ebp;
    int size;
    Data *val;
} Stack;

//globalni promenne, paska pro ulozeni instrukci, zasobnik a ukazatel na zacatek
Tape tape;
Stack stack;
PTapeItem main_pointer;

// stavy pro konecny automat nacitani cisel
enum {
    NSTART,
    NNUM,
    NESTATE,
    NFLOAT0,
    NFLOAT,
    NEXP0,
    NEXP,
    NEND,
} NumReadStates;

int init_stack(int size); /// inicializuje zasobnik
int pop_stack(int *dattype, univalue *value); /// popne ze zasobniku vrchni hodnotu, vrati take jeji typ
int push_stack(int dattype, univalue value); /// pushne na zasobnik hodnotu i jeji datovy typ
int delete_stack(); /// smaze cely zasobnik

int read_number(); /// nacte cislo ze stdin

#ifdef DEBUG
int print_stack(); /// vypise zasobnik
int print_tape(); /// vypise pasku instrukci
#endif

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
        (tape.bot -> next) = item;
        tape.bot = item;
    }
    
    return 0;
}

int delete_tape() /// smaze pasku
{
    PTapeItem item = tape.top;
    while (item != NULL) { // dokud neni na konci pasky
        PTapeItem temp = item;
        item = item -> next;
        free(temp);
    }

    tape.top = tape.bot = tape.act = NULL;

    return 0;
}

PTapeItem actnext_tape() /// vrati aktualni prvek a posune se na dalsi instrukci
{
    if (tape.act == NULL) return NULL;
    PTapeItem temp = tape.act;
    tape.act = tape.act -> next; // posunu se na dalsi instrukci
    return temp;
}

PTapeItem generate(int instr, void *adr) /// funkce, ktera prebere typ instrukce a ukazatel a vygeneruje podle toho instrukci
{
    PTapeItem item = malloc(sizeof(struct TTapeItem));
    if (item == NULL) return NULL;
    item -> instr = instr;
    item -> adr = adr;
    item -> next = NULL;
    add_to_tape(item); // prida na konec pasky
    return item;
}

int execute() /// funkce, ktera vezme instrukce z globalni tabulky prvku a vykona je
{
    init_stack(STACK_DEFAULT_SIZE); // standardne 1000 prvku, moznost realokace

    if (tape.top == NULL) { // paska instrukci je prazdna
        delete_stack();
        delete_tape();
        return 0;
    }

    tape.act = main_pointer;
    
    while (1) { // reknete mi duvod, proc by to nemohl byt nekonecny cyklus
        
        PTapeItem instr = actnext_tape(); // jdu na dalsi instrukci

        if (instr == NULL) ExecError(ERROR_INT_BAD_INSTR);

        switch (instr -> instr) {
        case IHALT: { // konec programu
            delete_tape();
            delete_stack();
            return 0;
            break; // sem by to nemelo dojit
        }
        case IJMP: { // podmineny skok, jenom nastavim aktualni prvek
            tape.act = (PTapeItem) instr -> adr;
            break;
        }
        case IJMPT: {
            univalue value;
            int dattype;
            try_pop_stack(dattype, value); // popnu
            // pokud bylo true, skocim
            if (dattype != DBOOL || value.log == STRUE)
                tape.act = (PTapeItem) instr -> adr;
            if (dattype == DSTRING) free(value.str);
            break;
        }
        case IJMPF: {
            univalue value;
            int dattype;
            try_pop_stack(dattype, value); // popnu
            // pokud bylo false, skocim
            if ((dattype == DBOOL && value.log == SFALSE) || dattype == DNIL)
                tape.act = (PTapeItem) instr -> adr;
            if (dattype == DSTRING) free(value.str);
            break;
        }
        case ICALL: {
            univalue value;
            int localvars = ((FunctionTree *) instr -> adr) -> vars;
            if ((stack.esp + localvars) >= stack.size) { // musim realokovat
                Data *temp_val = realloc(stack.val, (stack.size)*2*sizeof(Data));
                if (temp_val == NULL) ExecError(ERROR_GEN_MEM);
                stack.val = temp_val;
                stack.size *= 2;
            }
            // udelam misto na zasobniku a inicializuji
            for (int i = 0; i<localvars; i++) {
                stack.esp++;
                stack.val[stack.esp].type = DNIL;
                stack.val[stack.esp].value.log = STRUE;
            }
            //push retadr
            value.adr = (void *) instr -> next;
            try_push_stack(DRETADR, value, ERROR_GEN_MEM);
            //push ebp
            value.log = stack.ebp;
            try_push_stack(DREGISTER, value, ERROR_GEN_MEM);
            //mov ebp, esp
            stack.ebp = stack.esp;
            //jmp adr
            tape.act = ((FunctionTree *) instr -> adr) -> adr;
            break;
        }
        case IRET: {
            // v zasade musi popnout jednu vec, to bude navratova hodnota, pak bude popovat
            // dokud nebude na EBP, to obnovi, pak si ulozi navratovou adresu funkce
            // a pak bude popovat, podle poctu promennych a argumentu. pote pushne navratovou hodnotu a
            // EIP prepise navratovou adresou
            univalue retvalue, tempvalue;
            int rettype, temptype;
            // jedna navratova adresa
            try_pop_stack(rettype, retvalue);
            // vypopuju bordel, ktery by tam nemel byt, ale radsi...
            do {
                try_pop_stack(temptype, tempvalue);
                if (temptype == DSTRING) free(tempvalue.str);
            } while (temptype != DREGISTER); // az po EBP
            stack.ebp = tempvalue.log;
            try_pop_stack(temptype, tempvalue);
            // obnoveni navratove adresy
            if (temptype != DRETADR) {
                if (temptype == DSTRING) free(tempvalue.str);
                ExecError(ERROR_INT_BAD_VAL);
            }
            // nastaveni adresy dalsi instrukce
            tape.act = (PTapeItem) tempvalue.adr;
            // pocet prvku, co musim na zasobniku uvolnit
            int space = ((FunctionTree *) instr -> adr) -> vars + ((FunctionTree *) instr -> adr) -> params;
            for (int i = 0; i<space; i++) {
                try_pop_stack(temptype, tempvalue);
                if (temptype == DSTRING) free(tempvalue.str);
            }
            try_push_stack(rettype, retvalue, ERROR_GEN_MEM);
            break;
        }
        case IPUSH: {
            LiteralTree *literal = (LiteralTree *) instr -> adr;
            if (literal == NULL) ExecError(ERROR_INT_BAD_VAL); // nevim co pushnout
            if (literal -> data.type == T_STRING) { // string se musi kopirovat
                univalue str;
                str.str = malloc(sizeof(char)*(strlen(literal -> data.value.str)+1));
                if (str.str == NULL) ExecError(ERROR_GEN_MEM);
                strcpy(str.str, literal -> data.value.str);
                // T_TYP se prevede na DTYP, jsou kompatibilni
                free_try_push_stack(literal -> data.type, str, ERROR_GEN_MEM, str.str);
            } else
                try_push_stack(literal -> data.type, literal -> data.value, ERROR_GEN_MEM);
            break;
        }
        case IPUSHI: {
            Data ident;
            univalue retvalue;
            // ziskam offset lokalni promenne
            int offset = ((LocalTree*) (instr -> adr)) -> offset;
            // ziskam hodnotu
            ident = stack.val[stack.ebp+offset];
            if (ident.type == T_STRING) { // musim kopirovat
                retvalue.str = malloc(sizeof(char)*(strlen(ident.value.str) + 1));
                if (retvalue.str == NULL) ExecError(ERROR_GEN_MEM);
                strcpy(retvalue.str, ident.value.str);
                free_try_push_stack(ident.type, retvalue, ERROR_GEN_MEM, retvalue.str);
            } else {
                retvalue = ident.value;
                try_push_stack(ident.type, retvalue, ERROR_GEN_MEM);
            }
            break;
        }
        case IPUSHT: {
            univalue value;
            value.log = STRUE;
            try_push_stack(DBOOL, value, ERROR_GEN_MEM);
            break;
        }
        case IPUSHF: {
            univalue value;
            value.log = SFALSE;
            try_push_stack(DBOOL, value, ERROR_GEN_MEM);
            break;
        }
        case IPOP: {
            univalue value;
            int dattype;
            try_pop_stack(dattype, value);
            // vysledek nepouziji, nepotrebuji ho
            if (dattype == DSTRING) {
                free(value.str);
            }
            break;
        }
        case IPOPI: {
            // tady se string kopirovat nemusi
            univalue value;
            int dattype;
            try_pop_stack(dattype, value);
            int offset = ((LocalTree *) (instr -> adr)) -> offset;
            // musim uvolnit, pokud v promenne byl string
            if (stack.val[stack.ebp+offset].type == DSTRING)
                free(stack.val[stack.ebp+offset].value.str);
            stack.val[stack.ebp+offset].type = dattype;
            stack.val[stack.ebp+offset].value = value;
            break;
        }
        case IPUSHN: {
            univalue value;
            value.log = STRUE; // zbytecne, ale nepotrebujeme warningy
            try_push_stack(DNIL, value, ERROR_GEN_MEM);
            break;
        }
        case IADD: {
            univalue value1, value2, retvalue;
            int dattype1, dattype2;
            // pop dvou hodnot
            if (pop_stack(&dattype1, &value1) != 0 || dattype1 != DNUM) {
                if (dattype1 == DSTRING) free(value1.str);
                ExecError(ERROR_INT_BADPARAM);
            }
            if (pop_stack(&dattype2, &value2) != 0 || dattype2 != DNUM) {
                if (dattype2 == DSTRING) free(value2.str);
                ExecError(ERROR_INT_BADPARAM);
            }
            // jejich soucet
            retvalue.num = value2.num + value1.num;
            try_push_stack(DNUM, retvalue, ERROR_GEN_MEM);
            break;
        }
        case ISUB: {
            univalue value1, value2, retvalue;
            int dattype1, dattype2;
            // pop dvou hodnot
            if (pop_stack(&dattype1, &value1) != 0 || dattype1 != DNUM) {
                if (dattype1 == DSTRING) free(value1.str);
                ExecError(ERROR_INT_BADPARAM);
            }
            if (pop_stack(&dattype2, &value2) != 0 || dattype2 != DNUM) {
                if (dattype2 == DSTRING) free(value2.str);
                ExecError(ERROR_INT_BADPARAM);
            }
            // jejich rozdil
            retvalue.num = value2.num - value1.num;
            try_push_stack(DNUM, retvalue, ERROR_GEN_MEM);
            break;
        }
        case IMUL: {
            univalue value1, value2, retvalue;
            int dattype1, dattype2;
            // pop dvou hodnot
            if (pop_stack(&dattype1, &value1) != 0 || dattype1 != DNUM) {
                if (dattype1 == DSTRING) free(value1.str);
                ExecError(ERROR_INT_BADPARAM);
            }
            if (pop_stack(&dattype2, &value2) != 0 || dattype2 != DNUM) {
                if (dattype2 == DSTRING) free(value2.str);
                ExecError(ERROR_INT_BADPARAM);
            }
            // jejich nasobek
            retvalue.num = value1.num * value2.num;
            try_push_stack(DNUM, retvalue, ERROR_GEN_MEM);
            break;
        }
        case IDIV: {
            univalue value1, value2, retvalue;
            int dattype1, dattype2;
            // pop dvou hodnot
            if (pop_stack(&dattype1, &value1) != 0 || dattype1 != DNUM) {
                if (dattype1 == DSTRING) free(value1.str);
                ExecError(ERROR_INT_BADPARAM);
            }
            if (pop_stack(&dattype2, &value2) != 0 || dattype2 != DNUM) {
                if (dattype2 == DSTRING) free(value2.str);
                ExecError(ERROR_INT_BADPARAM);
            }
            if (value1.num == 0) // osetreni deleni nulou
                ExecError(ERROR_INT_DIVZERO);
            // jejich podil
            retvalue.num = value2.num / value1.num;
            try_push_stack(DNUM, retvalue, ERROR_GEN_MEM);
            break;
        }
        case IPOW: {
            univalue value1, value2, retvalue;
            int dattype1, dattype2;
            // pop dvou hodnot
            if (pop_stack(&dattype1, &value1) != 0 || dattype1 != DNUM) {
                if (dattype1 == DSTRING) free(value1.str);
                ExecError(ERROR_INT_BADPARAM);
            }
            if (pop_stack(&dattype2, &value2) != 0 || dattype2 != DNUM) {
                if (dattype2 == DSTRING) free(value2.str);
                ExecError(ERROR_INT_BADPARAM);
            }
            // umocnim druhou na tu prvni
            retvalue.num = pow(value2.num, value1.num);
            try_push_stack(DNUM, retvalue, ERROR_GEN_MEM);
            break;
        }
        case ICONCAT: {
            univalue value1, value2, retvalue;
            int dattype1, dattype2;
            // pop dvou stringu
            if (pop_stack(&dattype1, &value1) != 0 || dattype1 != DSTRING)
                ExecError(ERROR_INT_BADPARAM);
            if (pop_stack(&dattype2, &value2) != 0 || dattype2 != DSTRING) {
                free(value1.str);
                ExecError(ERROR_INT_BADPARAM);
            }
            // konkatenace ze string.c
            retvalue.str = concat(value2.str, value1.str);
            free(value2.str);
            free(value1.str);
            if (retvalue.str == NULL) ExecError(ERROR_GEN_MEM);
            free_try_push_stack(DSTRING, retvalue, ERROR_GEN_MEM, retvalue.str);
            break;
        }
        case ICMP: {
            univalue value1, value2, retvalue;
            int dattype1, dattype2;
            // prvni hodnota
            try_pop_stack(dattype1, value1);
            if (pop_stack(&dattype2, &value2) != 0) { // pop se nepovedl
                if (dattype1 == DSTRING) free(value1.str);
                ExecError(ERROR_INT_EMPTY_STACK);
            }
            if (dattype1 != dattype2) { // ruzne datove typy
                if (dattype1 == DSTRING) free(value1.str);
                if (dattype2 == DSTRING) free(value2.str);
                retvalue.log = SFALSE;
            } else if (dattype1 == DNUM) { // dve cisla
                if (value1.num == value2.num)
                    retvalue.log = STRUE;
                else
                    retvalue.log = SFALSE;
            } else if (dattype1 == DBOOL) { // dva booleany
                if (value1.log == value2.log)
                    retvalue.log = STRUE;
                else
                    retvalue.log = SFALSE;
            } else if (dattype1 == DSTRING) { // dva stringy
                if (strcmp(value1.str, value2.str) == 0)
                    retvalue.log = STRUE;
                else
                    retvalue.log = SFALSE;
                free(value1.str);
                free(value2.str);
            } else if (dattype1 == DNIL) retvalue.log = STRUE; // nil
            try_push_stack(DBOOL, retvalue, ERROR_GEN_MEM);
            break;
        }
        case ICMPN: {
            univalue value1, value2, retvalue;
            int dattype1, dattype2;
            try_pop_stack(dattype1, value1); // pop prvniho cisla
            if (pop_stack(&dattype2, &value2) != 0) { // druhy pop se nepodaril
                if (dattype1 == DSTRING) free(value1.str);
                ExecError(ERROR_INT_EMPTY_STACK);
            }
            if (dattype1 != dattype2) { // ruzne datove typy
                if (dattype1 == DSTRING) free(value1.str);
                if (dattype2 == DSTRING) free(value2.str);
                retvalue.log = STRUE;
            } else if (dattype1 == DNUM) { // dve cisla
                if (value1.num != value2.num)
                    retvalue.log = STRUE;
                else
                    retvalue.log = SFALSE;
            } else if (dattype1 == DBOOL) { // dva booleany
                if (value1.log != value2.log)
                    retvalue.log = STRUE;
                else
                    retvalue.log = SFALSE;
            } else if (dattype1 == DSTRING) { // dva stringy
                if (strcmp(value1.str, value2.str) == 0)
                    retvalue.log = SFALSE;
                else
                    retvalue.log = STRUE;
                free(value1.str);
                free(value2.str);
            } else if (dattype1 == DNIL) retvalue.log = SFALSE; // nil
            try_push_stack(DBOOL, retvalue, ERROR_GEN_MEM);
            break;
        }
        case ICMPL: {
            compare(<);
            break;
        }
        case ICMPG: {
            compare(>);
            break;
        }
        case ICMPEL: {
            compare(<=);
            break;
        }
        case ICMPEG: {
            compare(>=);
            break;
        }
        case IWRITE: {
            univalue value;
            int dattype;
            try_pop_stack(dattype, value); // popnu hodnotu
            if (dattype == DNUM) printf("%g", value.num); // vypisu cislo
            else if (dattype == DSTRING) {
                printf("%s", value.str); // vypisu string
                free(value.str);
            } else {
                ExecError(ERROR_INT_BADPARAM); // nic jineho neumim
            }
            break;
        }
        case IREAD: {
            univalue value;
            LiteralTree *literal = (LiteralTree *) instr -> adr; // ziskam format
            if (literal -> data.type == T_NUMBER) { // read(n)
                string input;
                str_new(&input, STR_INIT_LEN);
                int num_of_chars = literal -> data.value.num;
                int c = EOF;
                for (int i = 0; i<num_of_chars; i++) { // nactu zadany pocet cisel
                    c = fgetc(stdin);
                    if (c == EOF) break;
                    str_push(&input, c);
                }
                if (c == EOF) { // byl EOF na konci
                    if (input.str[0] == '\0') { // a pred nim nebylo nic == nil
                        str_free(&input);
                        value.log = STRUE;
                        try_push_stack(DNIL, value, ERROR_GEN_MEM);
                    } else { // pred nim bylo neco rozumneho
                        value.str = input.str;
                        free_try_push_stack(DSTRING, value, ERROR_GEN_MEM, value.str);
                    }
                } else { // nebyl EOF, vse v poradku
                    value.str = input.str;
                    free_try_push_stack(DSTRING, value, ERROR_GEN_MEM, value.str);
                }
            } else if (literal -> data.type == T_STRING){ // read("*x");
                char *fmt = literal -> data.value.str;
                if (strcmp(fmt, "*n") == 0) { // nacitam cislo
                    if (read_number() == ERROR_GEN_MEM) return ERROR_GEN_MEM;
                } else if (strcmp(fmt, "*l") == 0) { // nacitam do konce radku
                    string input;
                    str_new(&input, STR_INIT_LEN);
                    int c;
                    do {
                        c = fgetc(stdin);
                        if (c != '\n' && c != EOF) str_push(&input, c);
                    } while (c != '\n' && c != EOF);
                    if (c == EOF) { // na konci nebyl EOL ale EOF
                        if (input.str[0] == '\0') { // a nic jineho == nil
                            str_free(&input);
                            value.log = STRUE;
                            try_push_stack(DNIL, value, ERROR_GEN_MEM);
                        } else { // neco pred nim bylo, v poradku
                            value.str = input.str;
                            free_try_push_stack(DSTRING, value, ERROR_GEN_MEM, value.str);
                        }
                    } else { // v poradku
                        value.str = input.str;
                        free_try_push_stack(DSTRING, value, ERROR_GEN_MEM, value.str);
                    }
                } else if (strcmp(fmt, "*a") == 0) { // nacitam do EOF
                    string input;
                    str_new(&input, STR_INIT_LEN);
                    int c;
                    do {
                        c = fgetc(stdin);
                        if (c != EOF) str_push(&input, c);
                    } while (c != EOF);
                    value.str = input.str;
                    free_try_push_stack(DSTRING, value, ERROR_GEN_MEM, value.str);
                } else { // spatny formatovaci retezec
                    value.log = STRUE;
                    try_push_stack(DNIL, value, ERROR_GEN_MEM);
                }
            } else { // spatny argument
                value.log = STRUE;
                try_push_stack(DNIL, value, ERROR_GEN_MEM);
            }
            break;
        }
        case ITYPE: {
            univalue value, retvalue;
            int dattype;
            try_pop_stack(dattype, value);
            switch (dattype) { // podle datoveho typu pushnu string
            case DNUM:
                retvalue.str = malloc(sizeof(char)*7);
                if (retvalue.str == NULL) ExecError(ERROR_GEN_MEM);
                strcpy(retvalue.str, "number");
                break;
            case DSTRING:
                retvalue.str = malloc(sizeof(char)*7);
                if (retvalue.str == NULL) {
                    free(value.str);
                    ExecError(ERROR_GEN_MEM);
                }
                strcpy(retvalue.str, "string");
                free(value.str);
                break;
            case DBOOL:
                retvalue.str = malloc(sizeof(char)*8);
                if (retvalue.str == NULL) ExecError(ERROR_GEN_MEM);
                strcpy(retvalue.str, "boolean");
                break;
            case DNIL:
                retvalue.str = malloc(sizeof(char)*4);
                if (retvalue.str == NULL) ExecError(ERROR_GEN_MEM);
                strcpy(retvalue.str, "nil");
                break;
            default:
                // Na vrcholu zasobniku bylo neco jineho, chyba!
                ExecError(ERROR_INT_BAD_VAL);
            }
            try_push_stack(DSTRING, retvalue, ERROR_GEN_MEM);
            break;
        }
        case ISUBSTR: {
            univalue i, j, str, fin;
            int dattype1, dattype2, dattype3;
            try_pop_stack(dattype2, j); // prvni cislo
            if (dattype2 != DNUM) { // spatny datovy typ
                if (dattype2 == DSTRING) free(j.str);
                fin.log = STRUE;
                try_push_stack(DNIL, fin, ERROR_GEN_MEM);
                break;
            }
            try_pop_stack(dattype1, i); // druhe cislo
            if (dattype1 != DNUM) { // druhe cislo
                if (dattype1 == DSTRING) free(i.str);
                fin.log = STRUE;
                try_push_stack(DNIL, fin, ERROR_GEN_MEM);
                break;
            }
            if (pop_stack(&dattype3, &str) != 0 || dattype3 != DSTRING) { // string
                fin.log = STRUE;
                try_push_stack(DNIL, fin, ERROR_GEN_MEM);
                break;
            }
            string conv;
            if (str_init(&conv, str.str) != 1) {
                free(str.str);
                ExecError(ERROR_GEN_MEM);
            }
            fin.str = substr(&conv, i.num, j.num); //TODO: zkontrolovat, zda spravne pouzivam poradi indexu
            free(str.str);
            str_free(&conv);
            free_try_push_stack(DSTRING, fin, ERROR_GEN_MEM, fin.str);
            break;
        }
        case IFIND: {
            univalue str1, str2, retvalue;
            int dattype1, dattype2;
            // nacteni prvniho stringu
            if (pop_stack(&dattype2, &str2) != 0 || dattype2 != DSTRING) {
                retvalue.log = STRUE;
                try_push_stack(DNIL, retvalue, ERROR_GEN_MEM);
                break;
            }
            // nacteni druheho stringu
            if (pop_stack(&dattype1, &str1) != 0 || dattype1 != DSTRING) {
                free(str2.str);
                retvalue.log = STRUE;
                try_push_stack(DNIL, retvalue, ERROR_GEN_MEM);
                break;
            }
            string conv1, conv2;
            if (str_init(&conv1, str1.str) != 1) {
                free(str1.str);
                free(str2.str);
                ExecError(ERROR_GEN_MEM);
            }
            if (str_init(&conv2, str2.str) != 1) {
                free(str1.str);
                free(str2.str);
                str_free(&conv1);
                ExecError(ERROR_GEN_MEM);
            }
            free(str1.str);
            free(str2.str);
            retvalue.num = find(&conv1, &conv2); // zavolani find pomoci KMP algorimu
            str_free(&conv1);
            str_free(&conv2);
            try_push_stack(DNUM, retvalue, ERROR_GEN_MEM);
            break;
        }
        case ISORT: {
            univalue str, retvalue;
            int dattype;
            // popnu string
            if (pop_stack(&dattype, &str) != 0 || dattype != DSTRING) {
                retvalue.log = STRUE;
                try_push_stack(DNIL, retvalue, ERROR_GEN_MEM);
                break;
            }
            string conv;
            if (str_init(&conv, str.str) != 1) {
                free(str.str);
                retvalue.log = STRUE;
                try_push_stack(DNIL, retvalue, ERROR_GEN_MEM);
                break;
            }
            free(str.str);
            sort(&conv); // setridim
            retvalue.str = conv.str;
            free_try_push_stack(DSTRING, retvalue, ERROR_GEN_MEM, retvalue.str);
            break;
        }
        case INOP:
            break; // nedelat nic
        default:
            ExecError(ERROR_INT_BAD_INSTR);
        }

#ifdef DEBUG
        print_stack();
#endif
    }
    
    return 0;
}

int init_stack(int size) /// inicializuje zasobnik
{
    stack.val = malloc(sizeof(Data)*size);
    if (stack.val == NULL) return -1;
    stack.size = size;
    stack.esp = -1; // posledni pushnuty prvek
    stack.ebp = -1;
    return 0;
}

int pop_stack(int *dattype, univalue *value) /// popne ze zasobniku vrchni hodnotu, vrati take jeji typ
{
    if (stack.esp == -1) return -1;
    (*dattype) = stack.val[stack.esp].type;
    (*value) = stack.val[stack.esp].value;
    stack.esp--;
    
    return 0;
}

int push_stack(int dattype, univalue value) /// pushne na zasobnik hodnotu i jeji datovy typ
{
    if (stack.esp == (stack.size)-1) { // zasobnik je uz moc maly, realokovat
        Data *temp_val = realloc(stack.val, (stack.size)*2*sizeof(Data));
        if (temp_val == NULL) return -2;
        stack.val = temp_val;
        stack.size *= 2;
    }

    stack.esp++;
    stack.val[stack.esp].type = dattype;
    stack.val[stack.esp].value = value;
    
    return 0;
}

int delete_stack() /// smaze cely zasobnik
{
    for (int i = 0; i<=stack.esp; i++) {
        if (stack.val[i].type == DSTRING) // stringy musim uvolnovat
            free(stack.val[i].value.str);
    }
    free(stack.val);
    stack.esp = -1;
    stack.val = NULL;
    return 0;
}

int read_number()
{
    //TODO: nacitani cisla jeste nebude jen tak
    string input;
    str_new(&input, STR_INIT_LEN);
    int state = NSTART;
    while (state != NEND) { // statovy automat stejny jako v lex.
        
        int in;
        
        switch (state){
        case NSTART:
            in = fgetc(stdin);
            if (isspace(in)) { // preskakuji bile znaky na zacatku
                state = NSTART;
            } else if (!isdigit(in)) { // nebylo nactene zadne cislo
                str_free(&input);
                ungetc(in, stdin);
                univalue value;
                value.log = STRUE;
                try_push_stack(DNIL, value, ERROR_GEN_MEM);
                state = NEND;
            } else { // nactena prvni cislice
                str_push(&input, in);
                state = NNUM;
            }
            break;
        case NNUM:
            in = fgetc(stdin);
            if (isdigit(in)) { // nacitam dalsi cislici
                str_push(&input, in);
                state = NNUM;
            } else if (in == 'e' || in == 'E') { // exp. tvar
                str_push(&input, in);
                state = NESTATE;
            } else if (in == '.') { // desetinne cislo
                str_push(&input, in);
                state = NFLOAT0;
            } else { // konec cisla
                ungetc(in, stdin);
                univalue value;
                value.num = strtod(input.str, NULL);
                str_free(&input);
                try_push_stack(DNUM, value, ERROR_GEN_MEM);
                state = NEND;
            }
            break;
        case NESTATE: // nacteno cislo a e
            in = fgetc(stdin);
            if (isdigit(in)) { // pokracuji v cisle
                str_push(&input, in);
                state = NEXP;
            } else if (in == '+' || in == '-') { // exponent ma znamenko
                str_push(&input, in);
                state = NEXP0;
            } else { // nacetl jsem neco spatneho, neni to cislo
                ungetc(in, stdin);
                str_free(&input);
                univalue value;
                value.log = STRUE;
                try_push_stack(DNIL, value, ERROR_GEN_MEM);
                state = NEND;
            }
            break;
        case NFLOAT0: // nacteno cislo a desetinna tecka
            in = fgetc(stdin);
            if (isdigit(in)) { // desetinne cislo
                str_push(&input, in);
                state = NFLOAT;
            } else { // chyba
                ungetc(in, stdin);
                str_free(&input);
                univalue value;
                value.log = STRUE;
                try_push_stack(DNIL, value, ERROR_GEN_MEM);
                state = NEND;
            }
            break;
        case NFLOAT: // nacteno desetinne cislo
            in = fgetc(stdin);
            if (isdigit(in)) { // stale nacitam cisla
                str_push(&input, in);
                state = NFLOAT;
            } else if (in == 'e' || in == 'E') { // exp. tvar
                str_push(&input, in);
                state = NESTATE;
            } else { // konec nacitani cisla
                ungetc(in, stdin);
                univalue value;
                value.num = strtod(input.str, NULL);
                str_free(&input);
                try_push_stack(DNUM, value, ERROR_GEN_MEM);
                state = NEND;
            }
            break;
        case NEXP0: // nacteno cislo, e a znamenko
            in = fgetc(stdin);
            if (isdigit(in)) { // spravny exponencialni tvar
                str_push(&input, in);
                state = NEXP;
            } else { // spatny exponencialni tvar, e a + to bohuzel zezere
                ungetc(in, stdin);
                str_free(&input);
                univalue value;
                value.log = STRUE;
                try_push_stack(DNIL, value, ERROR_GEN_MEM);
                state = NEND;
            }
            break;
        case NEXP: // spravny exp. tvar, cekam na ukonceni
            in = fgetc(stdin);
            if (isdigit(in)) { // pridavam cisla
                str_push(&input, in);
                state = NEXP;
            } else { // konec cisla
                ungetc(in, stdin);
                univalue value;
                value.num = strtod(input.str, NULL);
                str_free(&input);
                try_push_stack(DNUM, value, ERROR_GEN_MEM);
                state = NEND;
            }
            break;
        default:
            break;
        }
    }

    return 0;
}

#ifdef DEBUG
int print_stack()
{
    printf("Zasobnik:\n----------\n");
    for (int i = 0; i<=(stack.esp); i++) {
        Data temp = stack.val[i];
        printf("* %d) <--\n", i+1);
        switch (temp.type) {
        case DNUM:
            printf("** cislo   %g\n", temp.value.num);
            break;
        case DSTRING:
            printf("** string  %s\n", temp.value.str);
            break;
        case DBOOL:
            printf("** boolean %d\n", temp.value.log);
            break;
        case DNIL:
            printf("** nil\n");
            break;
        case DREGISTER:
            printf("** ulozeny register\n");
            break;
        case DRETADR:
            printf("** navratova adresa funkce\n");
            break;
        default:
            printf("** !!! neco jineho\n");
        }
    }
    printf("----------\nkonec zasobniku\n\n");
    return 0;
}

int print_tape()
{
    //omg, hnuj
    char * i2s[] = {
    "IHALT  ",      
    "IJMP   ",       
    "IJMPT  ",      
    "IJMPF  ",      
    "ICALL  ",      
    "IRET   ",      
    "IPUSH  ",      
    "IPUSHI ",     
    "IPUSHT ",     
    "IPUSHF ",
    "IPOP   ",
    "IPOPI  ",      
    "IPUSHN ",     
    "IADD   ",       
    "ISUB   ",       
    "IMUL   ",       
    "IDIV   ",       
    "IPOW   ",       
    "ICONCAT",    
    "ICMP   ",       
    "ICMPN  ",      
    "ICMPL  ",      
    "ICMPG  ",      
    "ICMPEL ",     
    "ICMPEG ",     
    "IWRITE ",     
    "IREAD  ",      
    "ITYPE  ",      
    "ISUBSTR",    
    "IFIND  ",      
    "ISORT  ",      
    "INOP   "       
    };

    PTapeItem temp = tape.top;
    printf("Paska instrukci:\n---------------\n");
    while (temp != NULL) {
        printf("%p: %s %p \n",(void*) temp, i2s[temp -> instr], (void *) temp->adr);
        temp = temp -> next;
    }
    printf("\n---------------\nkonec pasky instrukci\n\n");
    return 0;
}
#endif
