#ifndef KVM_H
#define KVM_H

#include <stdint.h>
#include <linux/kvm.h>

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

	struct kvm_run *kvm_run;

	uint8_t is_running;
};

void create_kvm_vm(int fd);

void kvm_create_cpu(void);

void kvm_create_memory(size_t memsize);
void kvm_copy_code_to_memory(void *mem, int entry, uint8_t *code, size_t codesize);

int open_kvm(void);

#endif /* KVM_H */
