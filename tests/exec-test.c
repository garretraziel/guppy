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
    str.type = T_BOOLEAN;
    str.value.log = SFALSE;
    insert_literal(str);
    
    generate(IPUSH, last_literal);

    str.value.log = STRUE;
    insert_literal(str);

    generate(IPUSH, last_literal);
    
    generate(ICMP, NULL);
    generate(IHALT, NULL);

#ifdef DEBUG
    print_tape();
#endif
    
    execute();
    drop_literals();
}
