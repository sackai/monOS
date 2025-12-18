#include "idt.h"
#include <stdint.h>

#define IDT_ENTRIES 256

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr   idtp;

/* Assembly function that actually loads the IDT (defined in idt_load.asm) */
extern void idt_load(uint32_t);

/* Minimal memset implementation (we don't have libc) */
static void *memset32(void *dest, uint8_t value, uint32_t count) {
    uint8_t *d = (uint8_t *)dest;
    for (uint32_t i = 0; i < count; i++) {
        d[i] = value;
    }
    return dest;
}

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_lo = (uint16_t)(base & 0xFFFF);
    idt[num].base_hi = (uint16_t)((base >> 16) & 0xFFFF);

    idt[num].sel     = sel;
    idt[num].always0 = 0;
    idt[num].flags   = flags;   // present, ring 0, 32-bit interrupt gate
}

/* Set up a blank IDT and load it */
void idt_install(void) {
    idtp.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1;
    idtp.base  = (uint32_t)&idt;

    /* Clear the IDT */
    memset32(&idt, 0, sizeof(struct idt_entry) * IDT_ENTRIES);

    /* Later we'll call idt_set_gate() here to install ISRs */

    /* Load IDT using lidt instruction */
    idt_load((uint32_t)&idtp);
}
