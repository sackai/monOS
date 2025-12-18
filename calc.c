#include "console.h"
#include "string.h"
#include "input.h"
#include "calc.h"

// ----------------------
// Forward declarations
// ----------------------
static int parse_expression(const char* expr, int* out);
static int parse_expression_internal(int* out);
static int parse_term(int* out);
static int parse_factor(int* out);
static int parse_number(int* out);

static const char* p = "";   // parser pointer

// ----------------------
// Calculator state
// ----------------------
static char calc_buffer[128];
static int calc_len = 0;
static int calc_active = 0;


// ----------------------------------------------------
// Start calculator mode
// ----------------------------------------------------
void calc_shell() {
    calc_active = 1;
    calc_len = 0;

    set_input_mode(INPUT_MODE_CALC);

    kprint("Calculator mode. Type 'exit' to return.\n");
    kprint("calc> ");
}


// ----------------------------------------------------
// Handle per-character input
// ----------------------------------------------------
void calc_input(char c) {

    if (!calc_active) return;

    // ENTER → evaluate
    if (c == '\n') {
        calc_buffer[calc_len] = 0;
        terminal_put_char('\n');

        if (strcmp(calc_buffer, "exit") == 0) {
            kprint("Leaving calculator.\n");
            calc_active = 0;
            set_input_mode(INPUT_MODE_SHELL);
            kprint("AtariOS> ");
            return;
        }

        int result;
        if (parse_expression(calc_buffer, &result)) {
            char out[32];
            itoa(result, out);
            kprint(out);
        } else {
            kprint("Syntax error.");
        }

        calc_len = 0;
        kprint("\ncalc> ");
        return;
    }

    // BACKSPACE
    if (c == '\b') {
        if (calc_len > 0) {
            calc_len--;
            terminal_put_char('\b');
        }
        return;
    }

    // normal character
    if (calc_len < sizeof(calc_buffer) - 1) {
        calc_buffer[calc_len++] = c;
        terminal_put_char(c);
    }
}



// ----------------------------------------------------
// Expression Parser
// ----------------------------------------------------

static int parse_expression(const char* expr, int* out) {

    if (!expr) return 0;
    p = expr;

    if (!parse_expression_internal(out))
        return 0;

    return (*p == '\0');   // expression must end cleanly
}

// expr = term (('+' | '-') term)*
static int parse_expression_internal(int* out) {

    if (!parse_term(out)) return 0;

    while (*p == '+' || *p == '-') {

        char op = *p++;
        int rhs;

        if (!parse_term(&rhs)) return 0;

        if (op == '+') *out += rhs;
        else *out -= rhs;
    }

    return 1;
}

// term = factor (('*' | '/') factor)*
static int parse_term(int* out) {

    if (!parse_factor(out)) return 0;

    while (*p == '*' || *p == '/') {

        char op = *p++;
        int rhs;

        if (!parse_factor(&rhs)) return 0;

        if (op == '*') *out *= rhs;
        else {
            if (rhs == 0) return 0;
            *out /= rhs;
        }
    }

    return 1;
}

// factor = number | '(' expression ')'
static int parse_factor(int* out) {

    if (*p == '(') {
        p++; // skip '('

        if (!parse_expression_internal(out)) return 0;

        if (*p != ')') return 0;

        p++; // skip ')'
        return 1;
    }

    return parse_number(out);
}


// number
static int parse_number(int* out) {

    int value = 0;
    int digits = 0;

    while (*p >= '0' && *p <= '9') {
        value = value * 10 + (*p - '0');
        p++;
        digits++;
    }

    if (!digits) return 0;

    *out = value;
    return 1;
}
