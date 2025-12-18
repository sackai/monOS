#include <stdint.h>

void reboot() {
    asm volatile ("cli");
    asm volatile ("mov $0xFE, %al");
    asm volatile ("out %al, $0x64");
    while (1) { asm volatile ("hlt"); }
}
