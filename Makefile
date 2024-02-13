
.PHONY: all
all: kvmkvm kernel

.PHONY: kvmkvm
kvmkvm:
	@$(MAKE) -C hypervis

.PHONY: kernel
kernel:
	@$(MAKE) -C kernel

.PHONY:
clean:
	@rm -rf build bin