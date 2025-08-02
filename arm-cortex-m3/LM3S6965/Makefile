CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

TARGET = main
ASM_DIR = asms
BUILD_DIR = builds
SRC_DIR = sources

SRCS = $(wildcard $(SRC_DIR)/*.c)
#ASMS = $(patsubst %.c,$(SRC_DIR)/%.s,$(notdir $(SRCS)))
ASMS = $(patsubst %.c,$(ASM_DIR)/%.s,$(notdir $(SRCS)))
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(SRCS)))

CFLAGS = -mcpu=cortex-m3 -mthumb -Os -Wall -ffreestanding -nostdlib -nostartfiles
INCLUDES = -Iincludes
LDFLAGS = -T lm3s6965.ld -nostdlib

.PHONY: all asm clean help

all: $(BUILD_DIR) $(BUILD_DIR)/$(TARGET).bin

asm: $(ASM_DIR) $(ASMS)

$(ASM_DIR):
	mkdir -p $(ASM_DIR)

$(ASM_DIR)/%.s: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -S -fverbose-asm -o $@ $<

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	$(SIZE) $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(BUILD_DIR)

help:
	@echo "Available targets:"
	@echo "  all      - Build the project (default)"
	@echo "  asm      - Generate assembly (.s) files from C sources"
	@echo "  clean    - Remove all build files"
	@echo "  help     - Show this help message"
