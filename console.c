#include <stdint.h>
#include "console.h"

#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_ADDRESS 0xB8000

// IMPORTANT: use a constant, not a variable in .data !!
#define VGA_BUFFER ((uint16_t*)VGA_ADDRESS)

static uint8_t terminal_color;
static uint8_t terminal_row;
static uint8_t terminal_column;

static inline uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

void terminal_setcolor(uint8_t fg, uint8_t bg) {
    terminal_color = (bg << 4) | (fg & 0x0F);
}

void terminal_clear(void) {
    terminal_row = 0;
    terminal_column = 0;

    for (uint16_t y = 0; y < VGA_HEIGHT; y++) {
        for (uint16_t x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[y * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_put_char_at(char c, uint8_t x, uint8_t y) {
    if (x >= VGA_WIDTH || y >= VGA_HEIGHT)
        return;

    VGA_BUFFER[y * VGA_WIDTH + x] = vga_entry(c, terminal_color);
}

static void terminal_scroll(void) {
    for (uint16_t y = 1; y < VGA_HEIGHT; y++) {
        for (uint16_t x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[(y - 1) * VGA_WIDTH + x] =
                VGA_BUFFER[y * VGA_WIDTH + x];
        }
    }

    uint16_t last = VGA_HEIGHT - 1;
    for (uint16_t x = 0; x < VGA_WIDTH; x++) {
        VGA_BUFFER[last * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
    }

    terminal_row = last;
    terminal_column = 0;
}

void terminal_put_char(char c) {
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
    } else {
        terminal_put_char_at(c, terminal_column, terminal_row);
        terminal_column++;
        if (terminal_column >= VGA_WIDTH) {
            terminal_column = 0;
            terminal_row++;
        }
    }

    if (terminal_row >= VGA_HEIGHT) {
        terminal_scroll();
    }
}

void kprint(const char* str) {
    while (*str) {
        terminal_put_char(*str++);
    }
}
