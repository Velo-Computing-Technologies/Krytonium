CFLAGS = -m32 -ffreestanding -fno-pie -fno-exceptions -fno-rtti -fno-stack-protector

# List all the new modular C++ files
OBJS = main.o login.o app.o terminal.o default.o system.o settings.o

all: run

boot.bin: boot.asm
	nasm -f bin boot.asm -o boot.bin

kernel_entry.o: kernel_entry.asm
	nasm -f elf32 kernel_entry.asm -o kernel_entry.o

%.o: %.cpp orion.h
	g++ $(CFLAGS) -c $< -o $@

kernel.bin: kernel_entry.o $(OBJS)
	ld -m elf_i386 -o kernel.bin -Ttext 0x1000 kernel_entry.o $(OBJS) --oformat binary

os_image.bin: boot.bin kernel.bin
	cat boot.bin kernel.bin > os_image.bin
	dd if=/dev/zero bs=512 count=130 >> os_image.bin

run: os_image.bin
	qemu-system-x86_64 -drive format=raw,file=os_image.bin

clean:
	rm -f *.bin *.o
