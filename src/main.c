#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "kvmkvm.h"
#include "kvm.h"

struct kvm kvm = {0};

int main(void)
{
	kvm.fd = open_kvm();

	create_kvm_vm(kvm.fd);

	// load code and entry point
	uint8_t code[] = "\xB0\x61\xBA\x17\x02\xEE\xB0\n\xEE\xF4";
	kvm.code = code;
	kvm.codesz = sizeof(kvm.code);

	// 1GB
	// TODO: make this configurable
	const size_t memory_size = 0x40000000;
	kvm_create_memory(memory_size);

	kvm_create_cpu();

	cleanup();
	return 0;
}

void cleanup(void)
{
	if (kvm.fd != -1) {
		close_kvm();
	}
}

void die(const char *msg)
{
	fprintf(stderr, "[kvmkvm/error]: %s!\n", msg);
	fprintf(stderr, "[kvmkvm/info]: Error not recoverable: exitting.\n");

	cleanup();
	exit(EXIT_FAILURE);
}