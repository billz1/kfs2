NAME := kfs2
BUILD := build
SRC := src
INC := include
GRUBDIR := grub

CC := gcc
LD := ld
NASM := nasm

CFLAGS := -ffreestanding -O2 -Wall -Wextra -Werror -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs -m32 -I$(INC)
LDFLAGS := -m elf_i386 -T $(SRC)/linker.ld

OBJS := \
	$(BUILD)/boot.o \
	$(BUILD)/gdt_load.o \
	$(BUILD)/kernel.o \
	$(BUILD)/terminal.o \
	$(BUILD)/kprintf.o \
	$(BUILD)/keyboard.o \
	$(BUILD)/string.o \
	$(BUILD)/gdt.o \
	$(BUILD)/stackdump.o \
	$(BUILD)/shell.o

.PHONY: all clean fclean re run run-iso iso image run-image

all: $(BUILD)/$(NAME).bin

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/boot.o: $(SRC)/boot.s | $(BUILD)
	$(NASM) -f elf32 $< -o $@

$(BUILD)/gdt_load.o: $(SRC)/gdt_load.s | $(BUILD)
	$(NASM) -f elf32 $< -o $@

$(BUILD)/%.o: $(SRC)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/$(NAME).bin: $(OBJS) $(SRC)/linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)
	@echo "Built: $@"

# ---- ISO (GRUB) ----
iso: $(BUILD)/$(NAME).bin
	rm -rf $(BUILD)/iso
	mkdir -p $(BUILD)/iso/boot/grub
	cp $(BUILD)/$(NAME).bin $(BUILD)/iso/boot/$(NAME).bin
	cp $(GRUBDIR)/grub.cfg $(BUILD)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $(BUILD)/$(NAME).iso $(BUILD)/iso

run: run-iso

run-iso: iso
	qemu-system-i386 -boot d -cdrom $(BUILD)/$(NAME).iso -m 256M

# ---- Disk image (raw) ----
image: $(BUILD)/$(NAME).bin
	bash tools/mkimage.sh "$(BUILD)/$(NAME).bin" "$(BUILD)/$(NAME).img"

run-image: image
	qemu-system-i386 -boot c -drive format=raw,file=$(BUILD)/$(NAME).img -m 256M

clean:
	rm -rf $(BUILD)/*.o $(BUILD)/iso

fclean: clean
	rm -rf $(BUILD)/*.bin $(BUILD)/*.iso $(BUILD)/*.img

re: fclean all
