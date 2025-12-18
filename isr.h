#ifndef ISR_H
#define ISR_H

#include <stdint.h>

typedef struct isr_regs {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} isr_regs_t;

void isr_install(void);
void isr_handler(isr_regs_t *r);   // <-- IMPORTANT: pointer, not value

#endif

