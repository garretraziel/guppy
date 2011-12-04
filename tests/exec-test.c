#include <stdio.h>

#include "exec.h"
#include "defines.h"
#include "ial.h"

int main(void) {
    init_tape();
    Data name;
    name.type = T_NUMBER;
    name.value.num = 1;
    insert_literal(name);
    generate(IPUSH, last_literal, ALITTABLE);
    generate(IWRITE, NULL, ANONE);
    generate(IHALT, NULL, ANONE);
    execute();
    drop_literals();
}
