# BuddyOS — top-level build
#
# Targets:
#   all         : build the shell (default)
#   os-image    : build the OS bootable image
#   run         : build and run in QEMU
#   shell       : build the userspace shell
#   boot        : build only the bootloader
#   kernel      : build only the kernel
#   clean       : remove all generated files (build/)

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

# Source directories (never written to)
BOOT_DIR   := boot
KERNEL_SRC := kernel/src

# All generated output lives under build/
BUILD_DIR      := build
SHELL_BUILD    := $(BUILD_DIR)/shell
BOOT_BUILD     := $(BUILD_DIR)/boot
KERNEL_BUILD   := $(BUILD_DIR)/kernel

SHELL_BIN  := $(BUILD_DIR)/mysh
BOOT_BIN   := $(BUILD_DIR)/boot.bin
KERNEL_ELF := $(BUILD_DIR)/kernel.elf
KERNEL_BIN := $(BUILD_DIR)/kernel.bin
OS_IMAGE   := $(BUILD_DIR)/os_image.bin

# Shell sources
SHELL_SRCS := $(sort $(wildcard shell/src/*/*.c))
SHELL_OBJS := $(patsubst shell/src/%.c,$(SHELL_BUILD)/%.o,$(SHELL_SRCS))

# Kernel objects
KERNEL_ENTRY  := $(BOOT_BUILD)/entry.o
BOOT_ASM_OBJS := $(BOOT_BUILD)/isr_stubs.o
KERNEL_C_SRCS := $(wildcard $(KERNEL_SRC)/manager/*.c $(KERNEL_SRC)/utils/*.c)
KERNEL_OBJS   := $(KERNEL_ENTRY) \
                 $(BOOT_ASM_OBJS) \
                 $(patsubst $(KERNEL_SRC)/%.c,$(KERNEL_BUILD)/%.o,$(KERNEL_C_SRCS))

all: shell

shell: $(SHELL_BIN)

boot: $(BOOT_BIN)

kernel: $(KERNEL_BIN)

os-image: boot kernel
	cat $(BOOT_BIN) $(KERNEL_BIN) > $(OS_IMAGE)
	@SIZE=$$(wc -c < $(OS_IMAGE)); \
	  if [ $$SIZE -lt 5120 ]; then \
	    dd if=/dev/zero bs=1 count=$$((5120 - $$SIZE)) >> $(OS_IMAGE) 2>/dev/null; \
	  fi

run: os-image
	$(QEMU) -drive format=raw,file=$(OS_IMAGE)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all shell boot kernel os-image run clean

$(SHELL_BIN): $(SHELL_OBJS) | $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(SHELL_BUILD)/%.o: shell/src/%.c | $(SHELL_BUILD)
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(BOOT_BIN): $(BOOT_DIR)/boot.asm | $(BUILD_DIR)
	$(NASM) -f bin $< -o $@

$(BOOT_BUILD)/entry.o: $(BOOT_DIR)/entry.asm | $(BOOT_BUILD)
	$(NASM) -f elf32 $< -o $@

$(BOOT_BUILD)/isr_stubs.o: $(BOOT_DIR)/isr_stubs.s | $(BOOT_BUILD)
	$(CC_KERNEL) -m32 -c -o $@ $<

$(KERNEL_BUILD)/%.o: $(KERNEL_SRC)/%.c | $(KERNEL_BUILD)
	mkdir -p $(dir $@)
	$(CC_KERNEL) -m32 -ffreestanding -nostdlib -c -o $@ $<

$(KERNEL_BIN): $(KERNEL_OBJS) | $(BUILD_DIR)
	$(LD_KERNEL) -m elf_i386 -e _start -Ttext 0x1000 -o $(KERNEL_ELF) $^
	$(OBJCOPY) -O binary $(KERNEL_ELF) $@

$(BUILD_DIR) $(SHELL_BUILD) $(BOOT_BUILD) $(KERNEL_BUILD):
	mkdir -p $@
