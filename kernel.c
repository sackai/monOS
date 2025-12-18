/*#include <stdint.h>
#include "console.h"
#include "idt.h"
#include "isr.h"

void kmain(void) {
    // Install IDT + CPU exception handlers
    idt_install();
    isr_install();

    // Console setup
    terminal_setcolor(0xA, 0x0);   // light green on black
    terminal_clear();

    kprint("AtariOS kernel booted successfully!\n");
    kprint("IDT + ISRs installed.\n\n");

    // Test scrolling
    for (int i = 0; i < 40; i++) {
        kprint("Line ");
        char c = '0' + (i % 10);
        terminal_put_char(c);
        kprint(" - AtariOS running...\n");
    }

    kprint("\nNow triggering DIV 0...\n");

    asm volatile (
        "mov $0, %eax\n"
        "mov $1, %ebx\n"
        "div %eax\n"
    );

}
*/

#include <stdint.h>
#include "console.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "keyboard.h"
#include "shell.h"
#include "fs.h"
#include "ata.h"
#include "input.h"



// Simple tick counter for timer IRQ
static uint32_t ticks = 0;

// Called on each timer interrupt (IRQ0)
static void timer_handler(void) {
    ticks++;

    // Print a dot every 50 ticks or so (roughly ~0.5s at 100Hz PIT, depends on QEMU)
    if (ticks % 50 == 0) {
        kprint(".");
    }
}

void kmain(void) {
    terminal_setcolor(0xA, 0x0);
    terminal_clear();
    kprint("AtariOS booting...\n");


    idt_install();
    isr_install();
    kprint("[OK] IDT + ISRs installed.\n");


    irq_install();
    kprint("[OK] IRQ installed\n");


    keyboard_install();
    kprint("[OK] Keyboard driver installed\n");


    //terminal_setcolor(0xA, 0x0);
    //terminal_clear();
    set_input_mode(INPUT_MODE_SHELL);

    fs_init();
    kprint("AAAA\n");
    kprint("BBBB\n");
    kprint("CCCC\n");
    kprint("DDDD\n");
 
    kprint("[OK] File system initialized\n");

    kprint("AtariOS kernel with IRQs...\n");
    
    shell_init();  // start the shell

    __asm__ volatile ("sti");

    while (1) {
        __asm__ volatile("hlt");
    }
}
