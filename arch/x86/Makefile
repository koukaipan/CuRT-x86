
TARGET = CuRT-x86.img

.SUFFIXES:.asm .bin .c .s .o .S
.PHONY:	all run clean

HW_DEP_ASM_SRC = 

HW_DEP_C_SRC = \
	device/console.c \
	device/io.c \
	device/keyboard.c \
	device/screen.c \
	idt.c \
	gdt.c \
	isr.c \
	irq.c \
	timer.c \
	init.c \
	port.c

HW_DEP_C_OBJ = $(HW_DEP_C_SRC:.c=.o)

HW_DEP_ASM_OBJ = \
	apic.o \
	ctx_sw.o \
	isr_asm.o \
	irq_asm.o \

ALL_OBJS = $(SHELL_OBJ) $(KERNEL_OBJ) $(LIB_OBJ) \
	$(HW_DEP_ASM_OBJ) $(HW_DEP_C_OBJ)

CFLAGS += -m32 -fno-stack-protector
LDFLAGS = -m elf_i386 -nostdlib

all: $(TARGET)

$(TARGET): boot/bootsect.bin kernel.bin
	dd if=/dev/zero of=$(TARGET) bs=512 count=2880
	dd if=boot/bootsect.bin of=$(TARGET) bs=512 seek=0 conv=notrunc
	dd if=kernel.bin of=$(TARGET) bs=512 seek=1 conv=notrunc

kernel.bin: $(ALL_OBJS)
	$(LD) $(LDFLAGS) --oformat binary -T link.ld -o $@ $^

# The elf binary image could be used in other boot loader, ex. grub
kernel.elf.bin: boot/entry.o $(ALL_OBJS)
	$(LD) $(LDFLAGS) --oformat elf32-i386 -T link.elf.ld -o $@ $^

%.bin : %.asm
	nasm -f bin $< -o $@

$(HW_DEP_C_OBJ) : %.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(KERNEL_OBJ) $(SHELL_OBJ) $(LIB_OBJ) : %.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o : %.asm
	nasm -f elf $< -o $@
	
%.o:%.S
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(ALL_OBJS)
	
run: $(TARGET)
	qemu -no-kvm -m 32 -fda $< -boot a
