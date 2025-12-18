#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>
#include "isr.h"   // for isr_regs_t

typedef void (*irq_handler_t)(void);

/* Set up PIC + install IRQ gates in IDT */
void irq_install(void);

/* Register a C handler for an IRQ (0–15) */
void irq_register_handler(uint8_t irq, irq_handler_t handler);

/* Called from assembly stubs (defined in irq.asm) */
void irq_handler(isr_regs_t *r);

#endif
