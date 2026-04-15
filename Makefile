# BuddyOS — top-level build (shell and future milestones)

# Targets:
#   all         : build the shell (default, as before)
#   os-image    : build the OS bootable image (bootloader + kernel)
#   run         : build and run the OS in QEMU
#   shell       : build the userspace shell (mysh)
#   boot        : build only the bootloader
#   kernel      : build only the kernel (raw binary)
#   clean       : remove all generated files


CC       ?= cc
CFLAGS   ?= -Wall -Wextra -std=c11 -g
CPPFLAGS ?= -Ishell/src/builtin -Ishell/src/executer -Ishell/src/history -Ishell/src/parser -Ishell/src/manager
LDFLAGS  ?=
NASM     ?= nasm
QEMU     ?= qemu-system-i386

CROSS_PREFIX := i686-elf-
CC_KERNEL    := $(CROSS_PREFIX)gcc
LD_KERNEL    := $(CROSS_PREFIX)ld
OBJCOPY      := $(CROSS_PREFIX)objcopy

BUILD_DIR    := build
SHELL_BUILD  := $(BUILD_DIR)/shell
BOOT_DIR     := boot
KERNEL_SRC   := kernel/src
KERNEL_BUILD := $(KERNEL_SRC)

SHELL_BIN    := $(BUILD_DIR)/mysh
BOOT_BIN     := $(BUILD_DIR)/boot.bin
KERNEL_BIN   := $(KERNEL_SRC)/kernel.bin
OS_IMAGE     := $(BUILD_DIR)/os_image.bin

SHELL_SRCS   := $(sort $(wildcard shell/src/*/*.c))
SHELL_OBJS   := $(patsubst shell/src/%.c,$(SHELL_BUILD)/%.o,$(SHELL_SRCS))

KERNEL_ENTRY  := $(KERNEL_SRC)/manager/entry.o
KERNEL_C_SRCS := $(wildcard $(KERNEL_SRC)/manager/*.c $(KERNEL_SRC)/utils/*.c)
KERNEL_ASM_SRCS := $(wildcard $(KERNEL_SRC)/utils/*.s)
KERNEL_OBJS   := $(KERNEL_ENTRY) \
                 $(patsubst $(KERNEL_SRC)/%.c,$(KERNEL_BUILD)/%.o,$(KERNEL_C_SRCS)) \
                 $(patsubst $(KERNEL_SRC)/%.s,$(KERNEL_BUILD)/%.o,$(KERNEL_ASM_SRCS))

all: shell

shell: $(SHELL_BIN)

$(SHELL_BIN): $(SHELL_OBJS) | $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(SHELL_BUILD)/%.o: shell/src/%.c | $(SHELL_BUILD)
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(SHELL_BUILD) $(BUILD_DIR):
	mkdir -p $@

boot: $(BOOT_BIN)

$(BOOT_BIN): $(BOOT_DIR)/boot.asm | $(BUILD_DIR)
	$(NASM) -f bin $< -o $@

kernel: $(KERNEL_BIN)

$(KERNEL_BUILD)/%.o: $(KERNEL_SRC)/%.c | $(KERNEL_BUILD)
	mkdir -p $(dir $@)
	$(CC_KERNEL) -m32 -ffreestanding -nostdlib -c -o $@ $<

$(KERNEL_BUILD)/%.o: $(KERNEL_SRC)/%.s | $(KERNEL_BUILD)
	$(CC_KERNEL) -m32 -c -o $@ $<

$(KERNEL_SRC)/manager/entry.o: $(KERNEL_SRC)/manager/entry.asm
	$(NASM) -f elf32 $< -o $@

$(KERNEL_BIN): $(KERNEL_OBJS)
	$(LD_KERNEL) -m elf_i386 -e _start -Ttext 0x1000 -o $(KERNEL_SRC)/kernel.elf $^
	$(OBJCOPY) -O binary $(KERNEL_SRC)/kernel.elf $@

$(KERNEL_BUILD):
	mkdir -p $@

os-image: boot kernel | $(BUILD_DIR)
	cat $(BOOT_BIN) $(KERNEL_BIN) > $(OS_IMAGE)
	@# Pad to at least 5 sectors (2560 bytes) so BIOS read of 4 kernel sectors succeeds
	@SIZE=$$(wc -c < $(OS_IMAGE)); if [ $$SIZE -lt 2560 ]; then dd if=/dev/zero bs=1 count=$$((2560 - $$SIZE)) >> $(OS_IMAGE) 2>/dev/null; fi

run: os-image
	$(QEMU) -drive format=raw,file=$(OS_IMAGE)

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(KERNEL_SRC)/*.o $(KERNEL_SRC)/*.elf $(KERNEL_SRC)/*.bin
	rm -f $(KERNEL_SRC)/manager/*.o $(KERNEL_SRC)/utils/*.o

.PHONY: all shell boot kernel os-image run clean