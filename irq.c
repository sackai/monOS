#include "irq.h"
#include "idt.h"

extern uint32_t irq_stub_table[];

static irq_handler_t irq_handlers[16];

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* Remap PIC 1 and PIC 2:
 * Master IRQs → 32–39
 * Slave  IRQs → 40–47
 */
static void pic_remap(void) {
    uint8_t a1 = inb(0x21);   // save masks
    uint8_t a2 = inb(0xA1);

    outb(0x20, 0x11);         // starts the initialization sequence (in cascade mode)
    outb(0xA0, 0x11);

    outb(0x21, 0x20);         // Master PIC vector offset = 0x20 (32)
    outb(0xA1, 0x28);         // Slave PIC vector offset  = 0x28 (40)

    outb(0x21, 0x04);         // tell Master PIC there is a slave PIC at IRQ2 (0000 0100)
    outb(0xA1, 0x02);         // tell Slave PIC its cascade identity (0000 0010)

    outb(0x21, 0x01);         // 8086/88 (MCS-80/85) mode
    outb(0xA1, 0x01);

    outb(0x21, a1);           // restore saved masks
    outb(0xA1, a2);
}

void irq_install(void) {
    pic_remap();

    for (int i = 0; i < 16; i++)
        irq_handlers[i] = 0;

    // Install IRQ gates in IDT: vectors 32–47
    for (uint8_t i = 0; i < 16; i++) {
        idt_set_gate(32 + i, irq_stub_table[i], 0x08, 0x8E);
    }

    // Unmask only IRQ0 (timer) and IRQ1 (keyboard) for now
    outb(0x21, 0xFC);   // 1111 1100 -> enable IRQ0, IRQ1
    outb(0xA1, 0xFF);   // disable all slave IRQs
}

void irq_register_handler(uint8_t irq, irq_handler_t handler) {
    if (irq < 16) {
        irq_handlers[irq] = handler;
    }
}

/* Called from irq_common_stub in irq.asm */
void irq_handler(isr_regs_t *r) {
    uint8_t irq = (uint8_t)(r->int_no - 32);

    if (irq < 16 && irq_handlers[irq]) {
        irq_handlers[irq]();   // call user handler (no arguments)
    }

    // Send EOI (end-of-interrupt) to PICs
    if (irq >= 8) {
        outb(0xA0, 0x20);      // slave
    }
    outb(0x20, 0x20);          // master
}
