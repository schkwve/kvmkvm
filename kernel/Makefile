TARGET := ../bin/kernel.bin

CFILES := $(shell find . -name "*.c")
ASMFILES := $(shell find . -name "*.s")
OBJ := $(CFILES:%.c=../build/kernel/%.o) $(ASMFILES:%.s=../build/kernel/%.o)

CFLAGS := -Wall -Werror -fPIE -pie -masm=intel -nostdlib -Os -I.
LDFLAGS := -ffreestanding -fno-builtin -Tlink.x

.PRECIOUS: %.o
all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@ -Xlinker --oformat=binary $(LDFLAGS)

../build/kernel/%.o: %.s
	@mkdir -p $(@D)
	$(AS) $^ -o $@

../build/kernel/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -MMD -MP $< -o $@
