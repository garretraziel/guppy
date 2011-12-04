#include <stdio.h>

#include "exec.h"
#include "defines.h"
#include "ial.h"

int main(void) {
    init_tape();
    Data name;
    generate(IHALT, NULL, ANONE);
    execute();
}
