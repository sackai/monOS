#ifndef KEYBOARD_H
#define KEYBOARD_H

// Install IRQ1 handler
void keyboard_install(void);

// Collect characters (called by shell & IRQ)
void keyboard_collect(char c);

// Blocking read: wait until user types ENTER and return string
void keyboard_read(char* buffer, int max);

#endif
