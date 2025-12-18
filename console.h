#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>

void terminal_setcolor(uint8_t fg, uint8_t bg);
void terminal_clear(void);
void terminal_put_char(char c);
void terminal_put_char_at(char c, uint8_t x, uint8_t y);
void kprint(const char* str);

#endif
