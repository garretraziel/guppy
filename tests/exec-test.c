#include <stdio.h>

#include "exec.h"

int init_stack(int);
int delete_stack();

int main(void) {
    init_tape();
    Data name;
    name.type = DNUM;
    name.value.num = 1;
    insert_literal(Data);
    generate(IHALT, NULL, ANONE);
    execute();
}
