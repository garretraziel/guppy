#include <stdio.h>

#include "exec.h"

int init_stack(int);
int delete_stack();

int main(void) {
    init_tape();
    init_stack(1000);
    delete_tape();
    delete_stack();
}
