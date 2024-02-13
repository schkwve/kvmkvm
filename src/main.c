#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/kvm.h>
#include <sys/ioctl.h>

#include "hypercall.h"
#include "log.h"
#include "kvmkvm.h"
#include "kvm.h"

struct kvm kvm = {0};

int main(void)
{
	kvm.fd = open_kvm();

	create_kvm_vm(kvm.fd);

	// load code and entry point
	uint8_t code[] = "H\xB8\x41\x42\x43\x44\x31\x32\x33\nj\bY\xBA\x17\x02\x00\x00\xEEH\xC1\xE8\b\xE2\xF9\xF4";
	kvm.code = code;
	kvm.codesz = sizeof(code);

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
				kvmkvm_log(LOG_EXIT, "KVM_EXIT_HLT\n");
				kvm.is_running = 0;
				break;
			case KVM_EXIT_IO:
				if (!check_iopl()) {
					kvmkvm_log(LOG_EXIT, "KVM_EXIT_SHUTDOWN\n");
					kvm.is_running = 0;
					break;
				}
				if (kvm.kvm_run->io.port & HP_NR_MARK) {
					switch (kvm.kvm_run->io.port) {
						case NR_HP_open:
							hp_handle_open();
							break;
						case NR_HP_read:
							hp_handle_readwrite(read);
							break;
						case NR_HP_write:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
							hp_handle_readwrite(write);
#pragma GCC diagnostic pop
							break;
						case NR_HP_close:
							hp_handle_close();
							break;
						case NR_HP_lseek:
							hp_handle_lseek();
							break;
						case NR_HP_exit:
							hp_handle_exit();
							break;
						case NR_HP_panic:
							hp_handle_panic();
							break;
						default:
							kvmkvm_log(LOG_INFO, "Unhandled I/O port: 0x%x\n", kvm.kvm_run->io.port);
					}
				}
				putchar(*(((char *)kvm.kvm_run) + kvm.kvm_run->io.data_offset));
				break;
			case KVM_EXIT_FAIL_ENTRY:
				kvmkvm_log(LOG_EXIT, "KVM_EXIT_FAIL_ENTRY (hardware_entry_failure_reason = 0x%llx)\n", kvm.kvm_run->fail_entry.hardware_entry_failure_reason);
				kvm.is_running = 0;
				break;
			case KVM_EXIT_INTERNAL_ERROR:
				kvmkvm_log(LOG_EXIT, "KVM_EXIT_INTERNAL_ERROR (suberror = 0x%x)\n", kvm.kvm_run->internal.suberror);
				kvm.is_running = 0;
				break;
			case KVM_EXIT_SHUTDOWN:
				kvmkvm_log(LOG_EXIT, "KVM_EXIT_SHUTDOWN\n");
				kvm.is_running = 0;
				break;
			default:
				kvmkvm_log(LOG_EXIT, "KVMKVM_UNKNOWN\n");
				kvm.is_running = 0;
				break;
		}
	}

	cleanup();
	return 0;
}

int check_iopl(void)
{
	struct kvm_regs regs;
	struct kvm_sregs sregs;

	ioctl(kvm.cpufd, KVM_GET_REGS, &regs);
	ioctl(kvm.cpufd, KVM_GET_SREGS, &sregs);

	return (sregs.cs.dpl <= ((regs.rflags >> 12) & 3));
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
	kvmkvm_log(LOG_ERROR, "%s!\n", msg);
	kvmkvm_log(LOG_INFO, "Error not recoverable, exitting.\n");

	cleanup();
	exit(EXIT_FAILURE);
}