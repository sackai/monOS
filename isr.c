// isr.c
#include "isr.h"
#include "idt.h"
#include "console.h"
#include <stdint.h>

extern uint32_t isr_stub_table[];

static const char* exception_messages[32] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Floating Point Error",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating Point",
    "Virtualization",
    "Security",
    "Triple Fault",
    "FPU Error",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr_install(void) {
    for (uint8_t i = 0; i < 32; i++) {
        idt_set_gate(i, isr_stub_table[i], 0x08, 0x8E);
    }
}

void isr_handler(isr_regs_t *r) {
    kprint("\nEXCEPTION: ");
    if (r->int_no < 32) {
        kprint(exception_messages[r->int_no]);
    } else {
        kprint("Unknown");
    }
    kprint("\nSystem Halted.\n");

    while (1) {
        __asm__ volatile ("hlt");
    }
}
