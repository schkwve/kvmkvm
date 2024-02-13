TARGET := ../bin/kvmkvm
CFILES := $(shell find . -name "*.c")
OBJ := $(CFILES:%.c=../build/kvmkvm/%.o)
CFLAGS := -Wall -Wextra -Wpedantic
DBG_CFLAGS :=

ifdef KVM_DEBUG
	CFLAGS += -DDEBUG
	DBG_CFLAGS += -g
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(@D)
	$(CC) $(OBJ) $(DBG_CFLAGS) -o $@

../build/kvmkvm/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

debug:
	$(MAKE) KVM_DEBUG=1

clean:
	rm -rf build $(TARGET)
