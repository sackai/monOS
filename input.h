#ifndef INPUT_H
#define INPUT_H

typedef enum {
    INPUT_MODE_SHELL,
    INPUT_MODE_CALC
} input_mode_t;

// global variable
extern input_mode_t input_mode;

// functions
void set_input_mode(input_mode_t mode);
input_mode_t get_input_mode(void);   // <-- MISSING FUNCTION

#endif
