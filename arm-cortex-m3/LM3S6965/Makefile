CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

TARGET = main
BUILD_DIR = build
SRC_DIR = src

SRCS = $(wildcard $(SRC_DIR)/*.c)
ASMS = $(patsubst %.c,$(SRC_DIR)/%.s,$(notdir $(SRCS)))
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(SRCS)))

CFLAGS = -mcpu=cortex-m3 -mthumb -Os -Wall -ffreestanding -nostdlib -nostartfiles
LDFLAGS = -T lm3s6965.ld -nostdlib

all: $(BUILD_DIR) $(BUILD_DIR)/$(TARGET).bin

asm: $(SRC_DIR) $(ASMS)

$(SRC_DIR)/%.s: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -S -fverbose-asm -o $@ $<

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	$(SIZE) $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(BUILD_DIR)