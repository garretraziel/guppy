#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exec.h"
#include "defines.h"
#include "ial.h"

char *strcpy(char *, char*);

int main(void) {
    
    init_tape();
    
    generate(IPUSHT, NULL);
    generate(IPUSHF, NULL);
    
    generate(ICMP, NULL);
    generate(IHALT, NULL);

#ifdef DEBUG
    print_tape();
#endif
    
    execute();
    drop_literals();
}
