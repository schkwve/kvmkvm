#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/kvm.h>
#include <sys/ioctl.h>

#include "kvmkvm.h"
#include "kvm.h"

struct kvm kvm = {0};

int main(void)
{
	kvm.fd = open_kvm();

	create_kvm_vm(kvm.fd);

	// load code and entry point
	uint8_t code[] = "\xB0\x61\xBA\x17\x02\xEE\xB0\x0A\xEE\xF4";
	kvm.code = code;
	kvm.codesz = sizeof(kvm.code);

	// 1GB
	// TODO: make this configurable
	const size_t memory_size = 0x40000000;
	kvm_create_memory(memory_size);

	kvm_create_cpu();

	kvm.is_running = 1;
	while (kvm.is_running) {
		ioctl(kvm.cpufd, KVM_RUN, NULL);
		switch (kvm.kvm_run->exit_reason) {
			case KVM_EXIT_HLT:
				fprintf(stderr, "[kvmkvm/exit]: KVM_EXIT_HLT\n");
				kvm.is_running = 0;
				break;
			case KVM_EXIT_IO:
				// TODO: check port/direction
				putchar(*(((char *)kvm.kvm_run) + kvm.kvm_run->io.data_offset));
				break;
			case KVM_EXIT_FAIL_ENTRY:
				fprintf(stderr, "[kvmkvm/exit]: KVM_EXIT_FAIL_ENTRY (hardware_entry_failure_reason = 0x%llx)\n", kvm.kvm_run->fail_entry.hardware_entry_failure_reason);
				kvm.is_running = 0;
				break;
			case KVM_EXIT_INTERNAL_ERROR:
				fprintf(stderr, "[kvmkvm/exit]: KVM_EXIT_INTERNAL_ERROR (suberror = 0x%x)\n", kvm.kvm_run->internal.suberror);
				kvm.is_running = 0;
				break;
			case KVM_EXIT_SHUTDOWN:
				fprintf(stderr, "[kvmkvm/exit]: KVM_EXIT_SHUTDOWN\n");
				kvm.is_running = 0;
				break;
			default:
				fprintf(stderr, "[kvmkvm/exit]: KVMKVM_UNKNOWN\n");
				kvm.is_running = 0;
				break;
		}
	}

	cleanup();
	return 0;
}

void cleanup(void)
{
	// close descriptors
	if (kvm.cpufd != -1) {
		close(kvm.cpufd);
	}
	if (kvm.vmfd != -1) {
		close(kvm.vmfd);
	}
	if (kvm.fd != -1) {
		close(kvm.fd);
	}
}

void die(const char *msg)
{
	fprintf(stderr, "[kvmkvm/error]: %s!\n", msg);
	fprintf(stderr, "[kvmkvm/info]: Error not recoverable: exitting.\n");

	cleanup();
	exit(EXIT_FAILURE);
}