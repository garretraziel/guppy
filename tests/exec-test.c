#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exec.h"
#include "defines.h"
#include "ial.h"

char *strcpy(char *, char*);

int main(void) {
    
    init_tape();
    
    Data str;
    str.type = T_STRING;
    str.value.str = malloc(13);
    strcpy(str.value.str,"mamamelemaso");
    insert_literal(str);
    
    generate(IPUSH, last_literal, ALITTABLE);

    str.value.str = malloc(5);
    strcpy(str.value.str,"mele");
    insert_literal(str);

    generate(IPUSH, last_literal, ALITTABLE);
    
    generate(IFIND, NULL, ANONE);
    generate(IWRITE, NULL, ANONE);
    generate(IHALT, NULL, ANONE);

#ifdef DEBUG
    print_tape();
#endif
    
    execute();
    drop_literals();
}
