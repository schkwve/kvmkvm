TARGET := kvmkvm
CFILES := $(shell find src -name "*.c")
OBJ := $(CFILES:%.c=build/%.o)
CFLAGS := -Wall -Wextra -Wpedantic
DBG_CFLAGS :=

ifdef KVM_DEBUG
	CFLAGS += -DDEBUG
	DBG_CFLAGS += -g
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(DBG_CFLAGS) -o $@

build/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

debug:
	$(MAKE) KVM_DEBUG=1

clean:
	rm -rf build $(TARGET)
