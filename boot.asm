[org 0x7c00]
KERNEL_OFFSET equ 0x1000

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x9000

    mov [BOOT_DRIVE], dl

    ; Load the Kernel using LBA (Extended Disk Read)
    mov ah, 0x42
    mov dl, [BOOT_DRIVE]
    mov si, disk_packet
    int 0x13
    jc disk_error

    ; Switch to VGA Graphics Mode (320x200, 256 colors)
    mov ah, 0x00
    mov al, 0x13
    int 0x10

    ; Switch to 32-bit Protected Mode
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:init_pm

disk_error:
    jmp $

align 4
disk_packet:
    db 0x10                 ; Size of packet
    db 0                    ; Reserved
    dw 127                  ; Read 127 sectors (~63.5 KB)
    dw KERNEL_OFFSET        ; Offset to load the kernel to
    dw 0x0000               ; Segment
    dq 1                    ; Start reading at LBA sector 1

gdt_start:
    dq 0x0                  ; Null descriptor
gdt_code:                   ; Code Segment
    dw 0xffff, 0x0, 0x0, 0x9a, 0xcf, 0x0 
gdt_data:                   ; Data Segment
    dw 0xffff, 0x0, 0x0, 0x92, 0xcf, 0x0 
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

[bits 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ebp, 0x90000
    mov esp, ebp

    call KERNEL_OFFSET      
    jmp $

BOOT_DRIVE db 0
times 510-($-$$) db 0
dw 0xaa55
