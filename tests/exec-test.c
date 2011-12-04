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
    str.value.str = malloc(7);
    strcpy(str.value.str,"\nahoj\n");
    insert_literal(str);
    
    generate(IPUSH, last_literal, ALITTABLE);
    
    Data number;
    number.type = T_NUMBER;
    number.value.num = 1;
    insert_literal(number);
    
    generate(IPUSH, last_literal, ALITTABLE);
    
    generate(IWRITE, NULL, ANONE);
    generate(IWRITE, NULL, ANONE);
    generate(IHALT, NULL, ANONE);
    
    execute();
    drop_literals();
}
