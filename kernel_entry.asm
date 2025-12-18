bits 32
global kernel_entry
extern kmain

kernel_entry:
    mov esp, 0x90000

    ; debug: write "AA" in top-left
    ; mov dword [0xB8000], 0x0F410F41

    


    call kmain

hang:
    hlt
    jmp hang
