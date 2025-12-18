; boot.asm — loads kernel to 0x0000:0x1000 and executes it in 32-bit protected mode
bits 16
org 0x7C00

start:
    cli

    ; Set up real-mode segments + stack
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Save BIOS boot drive number
    mov [BOOT_DRIVE], dl

    ; Print boot message
    mov si, msg
.print_loop:
    lodsb
    test al, al
    jz .done_print
    mov ah, 0x0E
    mov bh, 0
    mov bl, 0x07
    int 0x10
    jmp .print_loop
.done_print:

    ; Load the kernel to 0000:1000 (physical 0x1000)
    call load_kernel

    ; Load GDT and enable protected mode
    lgdt [GDT_DESC]

    mov eax, cr0
    or eax, 1         ; set PE bit
    mov cr0, eax

    ; Far jump to 32-bit mode: code segment 0x08, offset pm_entry
    jmp 0x08:pm_entry

; ======================================================
;    32-bit PROTECTED MODE ENTRY
; ======================================================
bits 32
pm_entry:
    mov ax, 0x10      ; data segment selector
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    mov esp, 0x90000  ; set up high stack

    ; Jump to kernel entry point at 0x1000
    jmp 0x08:0x00001000

halt:
    hlt
    jmp halt

; ======================================================
;   16-bit BIOS DISK LOAD ROUTINE
; ======================================================
bits 16
load_kernel:
    xor ax, ax
    mov es, ax
    mov bx, 0x1000      ; ES:BX = 0000:1000 destination

    mov ah, 0x02        ; int 13h read sectors
    mov al, 50         ; LOAD 20 SECTORS = 10 KB
    mov ch, 0           ; cylinder 0
    mov cl, 2           ; sector 2 (sector 1 is bootloader)
    mov dh, 0           ; head 0
    mov dl, [BOOT_DRIVE]

    int 0x13
    jc disk_fail        ; if CF=1 → error

    ret

disk_fail:
    mov si, disk_msg
.err_print:
    lodsb
    test al, al
    jz .hang
    mov ah, 0x0E
    mov bh, 0
    mov bl, 0x04       ; red text
    int 0x10
    jmp .err_print

.hang:
    hlt
    jmp .hang

; ======================================================
;   STRINGS & GDT
; ======================================================

msg db "Bootloader: loading kernel...",0
disk_msg db " DISK READ ERROR ",0
BOOT_DRIVE db 0

align 8
GDT:
    dq 0                    ; null descriptor

GDT_CODE:                   ; code segment
    dw 0xFFFF               ; limit low
    dw 0x0000               ; base low
    db 0x00                 ; base mid
    db 10011010b            ; access: present, ring0, code
    db 11001111b            ; granularity + limit high
    db 0x00                 ; base high

GDT_DATA:                   ; data segment
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b            ; access: present, ring0, data
    db 11001111b
    db 0x00

GDT_END:

GDT_DESC:
    dw GDT_END - GDT - 1
    dd GDT

; Pad to 512 bytes and add MBR signature
times 510 - ($ - $$) db 0
dw 0xAA55
