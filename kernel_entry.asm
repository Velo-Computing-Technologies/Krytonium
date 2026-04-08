[bits 32]
[extern kernel_main]

call kernel_main    ; Jump into our C++ main.cpp execution!
jmp $               ; Infinite loop if the kernel ever unexpectedly exits
