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

#include "exec.h"
#include "defines.h"
#include "string.h"
#include "ial.h"

#define ExecError() do {delete_stack(); delete_tape(); return -1;} while(0)
#define try_push_stack(type,val) do { if (push_stack(type, val) != 0) ExecError(); } while(0)

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

int init_stack(int size); /// inicializuje zasobnik
int pop_stack(int *dattype, univalue *value); /// popne ze zasobniku vrchni hodnotu, vrati take jeji typ
int push_stack(int dattype, univalue value); /// pushne na zasobnik hodnotu i jeji datovy typ
int delete_stack(); /// smaze cely zasobnik

#ifdef DEBUG
int print_stack();
int print_tape();
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
    while (item != NULL) {
        PTapeItem temp = item;
        item = item -> next;
        free(temp);
    }

    tape.top = tape.bot = tape.act = NULL;

    return 0;
}

PTapeItem actnext_tape() /// posune aktivni prvek na dalsi a vrati ho
{
    if (tape.act == NULL) return NULL;
    PTapeItem temp = tape.act;
    tape.act = tape.act -> next; // posunu se na dalsi instrukci
    return temp;
}

PTapeItem generate(int instr, void *adr) /// funkce, ktera prebere typ instrukce a ukazatel a vygeneruje podle toho instrukci
{
    //TODO: takto jednoduse to samozrejme nejde, ale aspon aby byl mustr. joa funkce by mohla vracet adresu te vygenerovane instrukce
    //TODO: jinak se asi bude muset switchovat podle typu instrukce a vytvaret vsechny ty dodatecny veci, co jsou k tomu potreba
    PTapeItem item = malloc(sizeof(struct TTapeItem));
    if (item == NULL) return NULL;
    item -> instr = instr;
    item -> adr = adr;
    item -> next = NULL;
    add_to_tape(item);    
    return item;
}
//TODO: spousta veci se opakuje a dala by se dat do nejakych maker, ale me uz je to proste vsechno jedno, budu jenom kopirovat
int execute() /// funkce, ktera vezme instrukce z globalni tabulky prvku a vykona je
{
    init_stack(STACK_DEFAULT_SIZE);

    if (tape.top == NULL) {
        delete_stack();
        delete_tape();
        return 0;
    }

    tape.act = main_pointer;
    
    while (1) { // reknete mi duvod, proc by to nemohl byt nekonecny cyklus
        
        PTapeItem instr = actnext_tape();

        if (instr == NULL) ExecError(); //TODO: neco se nepovedlo, popremyslet co by to mohlo byt za chybu

        switch (instr -> instr) {
        case IHALT: {
            delete_tape();
            delete_stack();
            return 0;
            break; // sem by to nemelo dojit
        }
        case IJMP: {
            tape.act = (PTapeItem) instr -> adr;
            break;
        }
        case IJMPT: {
            univalue value;
            int dattype;
            if (pop_stack(&dattype, &value) != 0 || dattype != DBOOL) ExecError();
            if (value.log == STRUE) tape.act = (PTapeItem) instr -> adr;
            break;
        }
        case IJMPF: {
            univalue value;
            int dattype;
            if (pop_stack(&dattype, &value) != 0 || dattype != DBOOL) ExecError();
            if (value.log == SFALSE) tape.act = (PTapeItem) instr -> adr;
            break;
        }
        case ICALL: {
            //TODO: tady se jeste zasmejeme
            break;
        }
        case IRET: {
            //TODO: tady taky
            // v zasade musi popnout jednu vec, to bude navratova hodnota, pak bude popovat
            // dokud nebude na EBP, to obnovi, pak si ulozi navratovou adresu funkce
            // a pak bude popovat, dokud nebude na zarazce. pote pushne navratovou hodnotu a
            // EIP prepise navratovou adresou
            break;
        }
        case IPUSH: {
            LiteralTree *literal = (LiteralTree *) instr -> adr;
            if (literal == NULL) ExecError();
            if (literal -> data.type == T_STRING) { //TODO: kontrolovat kopirovani toho stringu z tabulky literalu
                univalue str;
                str.str = malloc(sizeof(char)*(strlen(literal -> data.value.str)+1));
                if (str.str == NULL) ExecError();
                strcpy(str.str, literal -> data.value.str);
                try_push_stack(literal -> data.type, str); // T_TYP se prevede na DTYP, jsou kompatibilni
            } else
                try_push_stack(literal -> data.type, literal -> data.value);
            break;
        }
        case IPUSHI: {
            Data ident;
            univalue retvalue;
            int offset = ((LocalTree*) (instr -> adr)) -> offset;
            ident = stack.val[stack.ebp-offset]; //TODO: zkusit, jestli je ten offset spravny. tady kontrolovat zda nepristupuju za pole
            if (ident.type == T_STRING) {
                retvalue.str = malloc(sizeof(char)*(strlen(ident.value.str) + 1));
                if (retvalue.str == NULL) ExecError();
                strcpy(retvalue.str, ident.value.str);
            } else
                retvalue = ident.value;
            try_push_stack(ident.type, retvalue); //TODO: zkontrolovat kopirovani stringu
            break;
        }
        case IPUSHT: {
            univalue value;
            value.log = STRUE;
            try_push_stack(DBOOL, value);
            break;
        }
        case IPUSHF: {
            univalue value;
            value.log = SFALSE;
            try_push_stack(DBOOL, value);
            break;
        }
        case IPOPI: {
            // tady se string kopirovat nemusi
            univalue value;
            int dattype;
            if (pop_stack(&dattype, &value) != 0) ExecError();
            int offset = ((LocalTree *) (instr -> adr)) -> offset;
            stack.val[stack.ebp-offset].type = dattype;
            stack.val[stack.ebp-offset].value = value;
            break;
        }
        case IPUSHN: {
            univalue value;
            value.log = STRUE;
            try_push_stack(DNIL, value);
            break;
        }
        case IADD: {
            univalue value1, value2, retvalue;
            int dattype1, dattype2;
            if (pop_stack(&dattype1, &value1) != 0 || dattype1 != DNUM)
                ExecError(); //TODO: nema to nekdy vracet nil?
            if (pop_stack(&dattype2, &value2) != 0 || dattype2 != DNUM)
                ExecError();
            retvalue.num = value2.num + value1.num;
            try_push_stack(DNUM, retvalue);
            break;
        }
        case ISUB: {
            univalue value1, value2, retvalue;
            int dattype1, dattype2;
            if (pop_stack(&dattype1, &value1) != 0 || dattype1 != DNUM)
                ExecError(); //TODO: nema to nekdy vracet nil?
            if (pop_stack(&dattype2, &value2) != 0 || dattype2 != DNUM)
                ExecError();
            retvalue.num = value2.num - value1.num;
            try_push_stack(DNUM, retvalue);
            break;
        }
        case IMUL: {
            univalue value1, value2, retvalue;
            int dattype1, dattype2;
            if (pop_stack(&dattype1, &value1) != 0 || dattype1 != DNUM)
                ExecError(); //TODO: nema to nekdy vracet nil?
            if (pop_stack(&dattype2, &value2) != 0 || dattype2 != DNUM)
                ExecError();
            retvalue.num = value1.num * value2.num;
            try_push_stack(DNUM, retvalue);
            break;
        }
        case IDIV: {
            univalue value1, value2, retvalue;
            int dattype1, dattype2;
            if (pop_stack(&dattype1, &value1) != 0 || dattype1 != DNUM)
                ExecError(); //TODO: nema to nekdy vracet nil?
            if (pop_stack(&dattype2, &value2) != 0 || dattype2 != DNUM)
                ExecError();
            if (value1.num == 0)
                ExecError(); //TODO: tady mam vracet nejakou dulezitou chybu, ne? ne?
            retvalue.num = value2.num / value1.num;
            try_push_stack(DNUM, retvalue);
            break;
        }
        case IPOW: {
            univalue value1, value2, retvalue;
            int dattype1, dattype2;
            if (pop_stack(&dattype1, &value1) != 0 || dattype1 != DNUM)
                ExecError(); //TODO: nema to nekdy vracet nil?
            if (pop_stack(&dattype2, &value2) != 0 || dattype2 != DNUM)
                ExecError();
            retvalue.num = pow(value2.num, value1.num);
            try_push_stack(DNUM, retvalue);
            break;
        }
        case ICONCAT: {
            univalue value1, value2, retvalue;
            int dattype1, dattype2;
            if (pop_stack(&dattype1, &value1) != 0 || dattype1 != DSTRING)
                ExecError(); //TODO: nema to nekdy vracet nil?
            if (pop_stack(&dattype2, &value2) != 0 || dattype2 != DSTRING)
                ExecError();
            retvalue.str = concat(value2.str, value1.str);
            free(value2.str);
            free(value1.str);
            if (retvalue.str == NULL) ExecError();
            try_push_stack(DSTRING, retvalue);
            break;
        }
        case ICMP: {
            univalue value1, value2, retvalue;
            int dattype1, dattype2;
            if (pop_stack(&dattype1, &value1) != 0) ExecError();
            if (pop_stack(&dattype2, &value2) != 0) ExecError();
            if (dattype1 != dattype2) {
                if (dattype1 == DSTRING) free(value1.str);
                if (dattype2 == DSTRING) free(value2.str);
                retvalue.log = SFALSE;
            } else if (dattype1 == DNUM) {
                if (value1.num == value2.num)
                    retvalue.log = STRUE;
                else
                    retvalue.log = SFALSE;
            } else if (dattype1 == DBOOL) {
                if (value1.log == value2.log)
                    retvalue.log = STRUE;
                else
                    retvalue.log = SFALSE;
            } else if (dattype1 == DSTRING) {
                if (strcmp(value1.str, value2.str) == 0) //TODO: nemam to tady resit nejak vic?
                    retvalue.log = STRUE;
                else
                    retvalue.log = SFALSE;
                free(value1.str);
                free(value2.str);
            } else if (dattype1 == DNIL) retvalue.log = STRUE;
            try_push_stack(DBOOL, retvalue);
            break;
        }
        case ICMPN: {
            univalue value1, value2, retvalue;
            int dattype1, dattype2;
            if (pop_stack(&dattype1, &value1) != 0) ExecError();
            if (pop_stack(&dattype2, &value2) != 0) ExecError();
            if (dattype1 != dattype2) {
                if (dattype1 == DSTRING) free(value1.str);
                if (dattype2 == DSTRING) free(value2.str);
                retvalue.log = STRUE;
            } else if (dattype1 == DNUM) {
                if (value1.num != value2.num)
                    retvalue.log = STRUE;
                else
                    retvalue.log = SFALSE;
            } else if (dattype1 == DBOOL) {
                if (value1.log != value2.log)
                    retvalue.log = STRUE;
                else
                    retvalue.log = SFALSE;
            } else if (dattype1 == DSTRING) {
                if (strcmp(value1.str, value2.str) == 0) //TODO: nemam to tady resit nejak vic?
                    retvalue.log = SFALSE;
                else
                    retvalue.log = STRUE;
                free(value1.str);
                free(value2.str);
            } else if (dattype1 == DNIL) retvalue.log = SFALSE;
            try_push_stack(DBOOL, retvalue);
            break;
        }
        case ICMPL:
            break;
        case ICMPG:
            break;
        case ICMPEL:
            break;
        case ICMPEG:
            break;
        case IWRITE: {
            univalue value;
            int dattype;
            if (pop_stack(&dattype, &value) != 0) ExecError();
            if (dattype == DNUM) printf("%g", value.num);
            else if (dattype == DSTRING) {
                printf("%s", value.str);
                free(value.str);
            } else {
                ExecError(); //TODO: write jenom string nebo num, jaka chyba kdyz je neco jineho?
            }
            break;
        }
        case IREAD: {
            univalue value;
            LiteralTree *literal = (LiteralTree *) instr -> adr;
            char *fmt = literal -> data.value.str;
            //TODO: sakra, tady bude zase dva kybly sracek nacitani, tohle v C DOOPRAVDY nemam rad
            break;
        }
        case ITYPE: {
            //TODO: ukladat ten string do stromu literalu
            univalue value, retvalue;
            int dattype;
            if (pop_stack(&dattype, &value) != 0) ExecError();
            switch (dattype) {
            case DNUM:
                retvalue.str = malloc(sizeof(char)*7);
                strcpy(retvalue.str, "number");
                break;
            case DSTRING:
                retvalue.str = malloc(sizeof(char)*7);
                strcpy(retvalue.str, "string");
                free(retvalue.str);
                break;
            case DBOOL:
                retvalue.str = malloc(sizeof(char)*8);
                strcpy(retvalue.str, "boolean");
                break;
            case DNIL:
                retvalue.str = malloc(sizeof(char)*4);
                strcpy(retvalue.str, "nil");
                break;
            default:
                // Na vrcholu zasobniku bylo neco jineho, chyba!
                ExecError();
            }
            try_push_stack(DSTRING, retvalue);
            break;
        }
        case ISUBSTR: {
            univalue i, j, str, fin;
            int dattype1, dattype2, dattype3;
            if (pop_stack(&dattype2, &j) != 0 || dattype2 != DNUM) ExecError();
            if (pop_stack(&dattype1, &i) != 0 || dattype1 != DNUM) ExecError();
            if (pop_stack(&dattype3, &str) != 0 || dattype3 != DSTRING) ExecError();
            string conv;
            if (str_init(&conv, str.str) != 1) ExecError();
            fin.str = substr(&conv, i.num, j.num); //TODO: zkontrolovat, zda spravne pouzivam poradi indexu
            free(str.str);
            str_free(&conv);
            try_push_stack(DSTRING, fin);
            break;
        }
        case IFIND: {
            univalue str1, str2, retvalue;
            int dattype1, dattype2;
            if (pop_stack(&dattype2, &str2) != 0 || dattype2 != DSTRING) ExecError();
            if (pop_stack(&dattype1, &str1) != 0 || dattype1 != DSTRING) ExecError();
            string conv1, conv2;
            if (str_init(&conv1, str1.str) != 1) ExecError();
            if (str_init(&conv2, str2.str) != 1) ExecError();
            free(str1.str);
            free(str2.str);
            retvalue.num = find(&conv1, &conv2);
            str_free(&conv1);
            str_free(&conv2);
            try_push_stack(DNUM, retvalue);
            break;
        }
        case ISORT: {
            univalue str, retvalue;
            int dattype;
            if (pop_stack(&dattype, &str) != 0 || dattype != DSTRING) ExecError();
            string conv;
            if (str_init(&conv, str.str) != 1) ExecError();
            free(str.str);
            sort(&conv);
            retvalue.str = conv.str;
            try_push_stack(DSTRING, retvalue);
            break;
        }
        case INOP:
            break;
        default:
            //TODO: nedefinovana instrukce, POMOC!
            ExecError(); //TODO: co to ma vracet za chybu?
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
    if (stack.val == NULL) return -1; //TODO: err_mem
    stack.size = size;
    stack.esp = -1;
    stack.ebp = -1;
    return 0;
}

int pop_stack(int *dattype, univalue *value) /// popne ze zasobniku vrchni hodnotu, vrati take jeji typ
{
    if (stack.esp == -1) return -1; //TODO: definovat error
    (*dattype) = stack.val[stack.esp].type;
    (*value) = stack.val[stack.esp].value;
    //free(stack -> val[stack -> esp]); //TODO: opravdu uvolnovat
    stack.esp--;
    
    return 0;
}

int push_stack(int dattype, univalue value) /// pushne na zasobnik hodnotu i jeji datovy typ
{
    if (stack.esp == (stack.size)-1) {
        Data *temp_val = realloc(stack.val, (stack.size)*2);
        if (temp_val == NULL) return -2; //TODO: err_jezis_dosel_nam_zasobnik
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
        if (stack.val[i].type == DSTRING)
            free(stack.val[i].value.str);
    }
    free(stack.val);
    stack.esp = -1;
    stack.val = NULL;
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
    "IHALT",      
    "IJMP",       
    "IJMPT",      
    "IJMPF",      
    "ICALL",      
    "IRET",      
    "IPUSH",      
    "IPUSHI",     
    "IPUSHT",     
    "IPUSHF",     
    "IPOPI",      
    "IPUSHN",     
    "IADD",       
    "ISUB",       
    "IMUL",       
    "IDIV",       
    "IPOW",       
    "ICONCAT",    
    "ICMP",       
    "ICMPN",      
    "ICMPL",      
    "ICMPG",      
    "ICMPEL",     
    "ICMPEG",     
    "IWRITE",     
    "IREAD",      
    "ITYPE",      
    "ISUBSTR",    
    "IFIND",      
    "ISORT",      
    "INOP"       
    };
    PTapeItem temp = tape.top;
    printf("Paska instrukci:\n---------------\n");
    while (temp != NULL) {
        printf("I: %s    ", i2s[temp -> instr]);
        temp = temp -> next;
    }
    printf("\n---------------\nkonec pasky instrukci\n\n");
    return 0;
}
#endif
