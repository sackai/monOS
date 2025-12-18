; isr.asm — CPU exception handlers (0–31)
bits 32

global isr_stub_table
global isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7
global isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15
global isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23
global isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31

extern isr_handler    ; void isr_handler(isr_regs_t *r)

; Macro for exceptions WITHOUT CPU error code
%macro ISR_NOERR 2
%1:
    cli
    push dword 0        ; dummy error code
    push dword %2       ; interrupt number
    jmp isr_common_stub
%endmacro

; Macro for exceptions WITH CPU error code
%macro ISR_ERR 2
%1:
    cli
    push dword %2       ; interrupt number (CPU already pushed err_code)
    jmp isr_common_stub
%endmacro

; --- Exception stubs 0–31 ---
ISR_NOERR isr0,  0
ISR_NOERR isr1,  1
ISR_NOERR isr2,  2
ISR_NOERR isr3,  3
ISR_NOERR isr4,  4
ISR_NOERR isr5,  5
ISR_NOERR isr6,  6
ISR_NOERR isr7,  7
ISR_ERR   isr8,  8      ; double fault (has error code)
ISR_NOERR isr9,  9
ISR_ERR   isr10, 10
ISR_ERR   isr11, 11
ISR_ERR   isr12, 12
ISR_ERR   isr13, 13
ISR_ERR   isr14, 14
ISR_NOERR isr15, 15
ISR_NOERR isr16, 16
ISR_NOERR isr17, 17
ISR_NOERR isr18, 18
ISR_NOERR isr19, 19
ISR_NOERR isr20, 20
ISR_NOERR isr21, 21
ISR_NOERR isr22, 22
ISR_NOERR isr23, 23
ISR_NOERR isr24, 24
ISR_NOERR isr25, 25
ISR_NOERR isr26, 26
ISR_NOERR isr27, 27
ISR_NOERR isr28, 28
ISR_NOERR isr29, 29
ISR_NOERR isr30, 30
ISR_NOERR isr31, 31

; --- Common ISR handler ---
isr_common_stub:
    pusha                   ; push eax,ecx,edx,ebx,esp,ebp,esi,edi
    mov ax, ds
    push eax                ; save old DS

    mov ax, 0x10            ; kernel data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp                ; argument: pointer to isr_regs_t
    call isr_handler
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

; --- Stub pointer table for C side ---
isr_stub_table:
    dd isr0,  isr1,  isr2,  isr3,  isr4,  isr5,  isr6,  isr7
    dd isr8,  isr9,  isr10, isr11, isr12, isr13, isr14, isr15
    dd isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23
    dd isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31
