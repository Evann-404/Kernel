TARGET = os.bin
ARCH = i686-elf
CC = $(ARCH)-gcc
AS = $(ARCH)-as
LD = $(CC)
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -T $(SRC_DIR)/linker.ld  -ffreestanding -O2 -nostdlib
QEMU = qemu-system-i386

SRC_DIR = src
BUILD_DIR = build

SRCS = $(SRC_DIR)/kernel.c
ASMS = $(SRC_DIR)/boot.s
OBJS = $(BUILD_DIR)/boot.o $(BUILD_DIR)/kernel.o

all: run

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/boot.o: $(ASMS) | $(BUILD_DIR)
	$(AS) $< -o $@

$(BUILD_DIR)/kernel.o: $(SRCS) | $(BUILD_DIR)
	$(CC) -c $< -o $@ $(CFLAGS)

link: $(OBJS)
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/$(TARGET) $(OBJS) -lgcc

run: link
	$(QEMU) -kernel $(BUILD_DIR)/$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

debug: link
	$(QEMU) -s -S -kernel $(BUILD_DIR)/$(TARGET)

.PHONY: all clean link run debug