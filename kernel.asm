; kernel.asm - simple 32-bit kernel loaded at 0x00001000
; nasm -f bin kernel.asm -o kernel.bin

bits 32
org 0x00001000

kernel_start:
    ; Write a message directly into VGA text memory (0xB8000)
    mov edi, 0xB8000
    mov esi, msg

.print_loop:
    lodsb               ; AL = [ESI], ESI++
    test al, al
    jz .done

    mov [edi], al       ; write character
    inc edi
    mov byte [edi], 0x0A ; attribute: light green on black (ish)
    inc edi
    jmp .print_loop

.done:
    cli
.hang:
    hlt
    jmp .hang

msg db "Hello from the kernel loaded from disk!", 0

; Pad kernel to one full sector (512 bytes)
times 512 - ($ - $$) db 0
