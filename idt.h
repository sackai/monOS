#ifndef IDT_H
#define IDT_H

#include <stdint.h>

/* Each IDT entry is 8 bytes */
struct idt_entry {
    uint16_t base_lo;   // lower 16 bits of handler address
    uint16_t sel;       // kernel code segment selector
    uint8_t  always0;   // must be 0
    uint8_t  flags;     // type & attributes
    uint16_t base_hi;   // upper 16 bits of handler address
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

void idt_install(void);
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

#endif
