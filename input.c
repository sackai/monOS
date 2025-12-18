#include "input.h"

input_mode_t input_mode = INPUT_MODE_SHELL; // default

void set_input_mode(input_mode_t mode) {
    input_mode = mode;
}

input_mode_t get_input_mode(void) {   // <-- IMPLEMENT THIS
    return input_mode;
}
