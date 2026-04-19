TARGET ?= x86_64
VERSION ?= dev
BUILD_DIR := build/$(TARGET)
KERNEL_ELF := $(BUILD_DIR)/kernel.elf

ifeq ($(TARGET),x86_64)
CC := clang
LD := ld.lld
CFLAGS := -ffreestanding -fno-stack-protector -m64 -mno-red-zone -Ikernel/include -O2 -Wall -Wextra -DNEOOS_VERSION_STR=\"$(VERSION)\"
LDFLAGS := -T kernel/linker-x86_64.ld
QEMU := qemu-system-x86_64 -cdrom $(BUILD_DIR)/neoOS-x86_64.iso -m 512M
else ifeq ($(TARGET),aarch64)
CC := aarch64-linux-gnu-gcc
LD := aarch64-linux-gnu-ld
CFLAGS := -ffreestanding -fno-stack-protector -Ikernel/include -O2 -Wall -Wextra -DNEOOS_VERSION_STR=\"$(VERSION)\"
LDFLAGS := -T kernel/linker-aarch64.ld
QEMU := qemu-system-aarch64 -M virt -cpu cortex-a72 -m 1024 -kernel $(KERNEL_ELF) -nographic
else
$(error Unsupported TARGET=$(TARGET). Use x86_64 or aarch64)
endif

SRC := $(wildcard kernel/src/*.c)
OBJ := $(patsubst kernel/src/%.c,$(BUILD_DIR)/%.o,$(SRC))

.PHONY: all clean iso run help prepare-limine

all: $(KERNEL_ELF)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: kernel/src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_ELF): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

prepare-limine:
	./scripts/prepare_limine.sh

iso: all prepare-limine
	./scripts/build_iso.sh

run: iso
	$(QEMU)

help:
	@echo "Targets:"
	@echo "  make TARGET=x86_64 VERSION=dev - build x86_64 kernel"
	@echo "  make TARGET=aarch64       - build aarch64 kernel"
	@echo "  make prepare-limine       - fetch/copy Limine CD boot files"
	@echo "  make iso TARGET=x86_64    - build bootable x86_64 ISO"
	@echo "  make run TARGET=x86_64    - run x86_64 ISO in QEMU"

clean:
	rm -rf build
