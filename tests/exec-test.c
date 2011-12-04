#include <stdio.h>

#include "exec.h"

int init_stack(int);
int delete_stack();

int main(void) {
    init_tape();
    generate(IHALT, NULL, ANONE);
    execute();
}
