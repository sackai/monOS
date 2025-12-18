#include <stdint.h>
#include "irq.h"
#include "console.h"
#include "shell.h"
#include "input.h"
#include "calc.h"
#include "port_io.h"


static int shift = 0;


static const char scancode_to_ascii[128] = {
    0,27,'1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\',
    'z','x','c','v','b','n','m',',','.','/',0,'*',0,' '
};

static const char scancode_shift_ascii[128] = {
    0,27,'!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0,'A','S','D','F','G','H','J','K','L',':','"','~',0,'|',
    'Z','X','C','V','B','N','M','<','>','?',0,'*',0,' '
};

extern void shell_input(char c);
   // ADD THIS

static void keyboard_callback(void) {
    uint8_t scancode;
    __asm__ volatile("inb $0x60, %0" : "=a"(scancode));

    // SHIFT press
    if (scancode == 0x2A || scancode == 0x36) {
        shift = 1;
        goto end;
    }

    // SHIFT release
    if (scancode == 0xAA || scancode == 0xB6) {
        shift = 0;
        goto end;
    }

    // Ignore key releases
    if (scancode & 0x80)
        goto end;

    char c = shift
        ? scancode_shift_ascii[scancode]
        : scancode_to_ascii[scancode];

    /*if (c) {
        if (get_input_mode() == INPUT_MODE_CALC)
            calc_input(c);
        else
            shell_input(c);
    }*/
    if (c) {
        keyboard_collect(c);   // MODIFY THIS LINE
    }

end:
    // THIS LINE WAS MISSING
    outb(0x20, 0x20);   // Send EOI to PIC
}



void keyboard_install(void) {
    irq_register_handler(1, keyboard_callback);
}

// Line buffer
static char input_buffer[128];
static int  input_len = 0;
static int  input_ready = 0;



// Called by ISR code
void shell_input(char c);   // forward declare

// interrupt handler already calls shell_input()
// so we modify shell_input in shell.c to ALSO fill calculator buffer

void keyboard_read(char* out, int max) {
    // wait for a full line
    while (!input_ready) {
        __asm__ volatile("hlt");
    }

    // copy line
    int n = (input_len < max - 1) ? input_len : max - 1;
    for (int i = 0; i < n; i++)
        out[i] = input_buffer[i];

    out[n] = '\0';

    input_len = 0;
    input_ready = 0;
}

// Called for every key (in IRQ)
void keyboard_collect(char c) {
    if (c == '\n') {
        input_ready = 1;
        return;
    }

    if ((unsigned)input_len < sizeof(input_buffer) - 1) {
        input_buffer[input_len++] = c;
    }
}
