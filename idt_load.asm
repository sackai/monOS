; idt_load.asm - tiny helper to load IDT from C

bits 32
global idt_load

; void idt_load(uint32_t idt_ptr_address);
idt_load:
    mov eax, [esp + 4]   ; argument: pointer to struct idt_ptr
    lidt [eax]           ; load IDT register
    ret
