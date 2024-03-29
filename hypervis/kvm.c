#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/kvm.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "cpu.h"
#include "paging.h"
#include "kvm.h"
#include "kvmkvm.h"

void create_kvm_vm(int fd)
{
	kvm.vmfd = ioctl(fd, KVM_CREATE_VM, 0);
}

void kvm_create_cpu(void)
{
	kvm.cpufd = ioctl(kvm.vmfd, KVM_CREATE_VCPU, 0);

	// set up memory for the cpu
	kvm.vcpu_mmap_size = ioctl(kvm.fd, KVM_GET_VCPU_MMAP_SIZE, NULL);
	kvm.kvm_run = (struct kvm_run *)mmap(0, kvm.vcpu_mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, kvm.cpufd, 0);

	// set up registers
	ioctl(kvm.cpufd, KVM_GET_REGS, &kvm.regs);
	kvm.regs.rip = kvm.memuent;
	kvm.regs.rsp = PS_LIMIT;
	kvm.regs.rdi = PS_LIMIT;
	kvm.regs.rsi = MEM_SIZE - kvm.regs.rdi;
	kvm.regs.rflags = 0x2;
	ioctl(kvm.cpufd, KVM_SET_REGS, &kvm.regs);

	ioctl(kvm.cpufd, KVM_GET_SREGS, &kvm.sregs);
	kvm.sregs.cs.base = 0;
	kvm.sregs.cs.selector = 0;
	paging_create_pt(kvm.mem, &kvm.sregs);
	cpu_setup_segregs(&kvm.sregs);
	ioctl(kvm.cpufd, KVM_SET_SREGS, &kvm.sregs);
}

void kvm_create_memory(size_t memsize)
{
	kvm.memuent = 0x0;
	kvm.mem = mmap(0, memsize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	kvm.memsz = memsize;
	
	kvm_copy_code_to_memory(kvm.mem, kvm.memuent, kvm.code, kvm.codesz);

	struct kvm_userspace_memory_region region = {
		.slot = 0,
		.flags = 0,
		.guest_phys_addr = 0,
		.memory_size = memsize,
		.userspace_addr = (size_t)kvm.mem
	};

	ioctl(kvm.vmfd, KVM_SET_USER_MEMORY_REGION, &region);
}

void kvm_load_code(const char *filename, uint8_t **codep, size_t *sizep)
{
	FILE *kernel = fopen(filename, "rb");
	if (kernel == NULL) {
		die("Failed to open kernel");
	}
	fseek(kernel, 0, SEEK_END);
	size_t size = ftell(kernel);
	rewind(kernel);

	uint8_t *code = (uint8_t *)malloc(size);
	if (code == NULL) {
		die("Couldn't allocate memory for kernel");
	}

	fread(code, 1, size, kernel);

	fclose(kernel);
	*codep = code;
	*sizep = size;
}

void kvm_copy_code_to_memory(void *mem, int entry, uint8_t *code, size_t codesize)
{
	memcpy((void *)((size_t)mem + entry), code, codesize);
}

int open_kvm(void)
{
	int fd = open("/dev/kvm", O_RDWR | O_CLOEXEC);
	if (fd == -1) {
		die("failed to open /dev/kvm");
	}

	return fd;
}
