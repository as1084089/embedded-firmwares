CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

TARGET = lm3s6965
SRC_DIR = sources
BUILD_DIR = builds
ASM_DIR = asms

CFLAGS = -mcpu=cortex-m3 -mthumb -Os -Wall -ffreestanding -nostdlib -nostartfiles
INCLUDES = -Iincludes
LDFLAGS = -T lm3s6965.ld -nostdlib

SRCS = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*.s) $(wildcard $(SRC_DIR)/*.S)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
OBJS := $(OBJS:$(SRC_DIR)/%.s=$(BUILD_DIR)/%.o)
OBJS := $(OBJS:$(SRC_DIR)/%.S=$(BUILD_DIR)/%.o)

ASMS = $(SRCS:$(SRC_DIR)/%.c=$(ASM_DIR)/%.s)

.PHONY: all clean asm help

all: $(BUILD_DIR) $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(ASM_DIR):
	mkdir -p $(ASM_DIR)

# C file → object
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Assembly file (.s) → object
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
	$(CC) $(CFLAGS) -c $< -o $@

# Preprocessed Assembly file (.S) → object
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S
	$(CC) $(CFLAGS) -c $< -o $@

# C file → assembly (.s) output for analysis/debug
$(ASM_DIR)/%.s: $(SRC_DIR)/%.c | $(ASM_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -S -fverbose-asm -o $@ $<

# ELF linking
$(BUILD_DIR)/$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	$(SIZE) $@

# BIN output
$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

asm: $(ASM_DIR) $(ASMS)

clean:
	rm -rf $(BUILD_DIR) $(ASM_DIR)

help:
	@echo "Available targets:"
	@echo "  all      - Build everything (default)"
	@echo "  asm      - Generate assembly (.s) files from C sources (in $(ASM_DIR)/)"
	@echo "  clean    - Remove all build files"
	@echo "  help     - Show this help message"
