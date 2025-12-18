; irq.asm — IRQ (hardware interrupt) stubs for IRQ0–IRQ15
bits 32

global irq_stub_table
global irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7
global irq8, irq9, irq10, irq11, irq12, irq13, irq14, irq15

extern irq_handler        ; C function: void irq_handler(isr_regs_t *r)

; Macro to define a stub for a given IRQ that maps to vector NUM
%macro IRQ_STUB 2
%1:
    cli
    push dword 0          ; dummy error code to match isr_regs_t layout
    push dword %2         ; interrupt vector number (32–47)
    jmp irq_common_stub
%endmacro

; PIC IRQs → IDT vectors 32–47
IRQ_STUB irq0,  32
IRQ_STUB irq1,  33
IRQ_STUB irq2,  34
IRQ_STUB irq3,  35
IRQ_STUB irq4,  36
IRQ_STUB irq5,  37
IRQ_STUB irq6,  38
IRQ_STUB irq7,  39
IRQ_STUB irq8,  40
IRQ_STUB irq9,  41
IRQ_STUB irq10, 42
IRQ_STUB irq11, 43
IRQ_STUB irq12, 44
IRQ_STUB irq13, 45
IRQ_STUB irq14, 46
IRQ_STUB irq15, 47

; Common IRQ handler: save registers, call C irq_handler(), restore, return
irq_common_stub:
    pusha                   ; push eax,ecx,edx,ebx,esp,ebp,esi,edi
    mov ax, ds
    push eax                ; save old DS

    mov ax, 0x10            ; kernel data selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp                ; pointer to isr_regs_t
    call irq_handler
    add esp, 4

    pop eax                 ; restore old DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa
    add esp, 8              ; pop err_code + int_no
    sti
    iretd

; Table of stub addresses for C side (irq_install) to use
irq_stub_table:
    dd irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7
    dd irq8, irq9, irq10, irq11, irq12, irq13, irq14, irq15
