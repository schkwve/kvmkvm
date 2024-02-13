#ifndef KVM_H
#define KVM_H

#include <stdint.h>
#include <stddef.h>
#include <linux/kvm.h>

#define PS_LIMIT (0x200000)
#define KERNEL_STACK_SIZE (0x4000)

#define PT_SIZE (0x5000)
#define MEM_SIZE (PS_LIMIT * 0x02)

struct kvm {
	// /dev/kvm
	int fd;
	int vmfd;
	int cpufd;

	uint8_t *code;
	size_t codesz;

	size_t memsz;
	int memuent;
	void *mem;

	struct kvm_regs regs;
	struct kvm_sregs sregs;

	struct kvm_run *kvm_run;

	uint8_t is_running;
};

void create_kvm_vm(int fd);

void kvm_create_cpu(void);

void kvm_create_memory(size_t memsize);
void kvm_load_code(const char *filename, uint8_t **codep, size_t *sizep);
void kvm_copy_code_to_memory(void *mem, int entry, uint8_t *code, size_t codesize);

int open_kvm(void);

#endif /* KVM_H */
