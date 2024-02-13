#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "hypercall.h"
#include "log.h"
#include "kvmkvm.h"

static struct fd_handle fd_map[MAX_FD + 1];

static void init_fd_map(void)
{
	static int fd_map_init = 0;
	if (!fd_map_init) {
		fd_map_init = 1;
		fd_map[0].real_fd = 0;
		fd_map[0].opening = 1;
		fd_map[1].real_fd = 1;
		fd_map[1].opening = 1;
		fd_map[2].real_fd = 2;
		fd_map[2].opening = 1;
		for (int i = 3; i < MAX_FD; i++) {
			fd_map[i].opening = 0;
		}
	}
}

void hp_handle_open(void)
{
	static int ret = UNUSED;

	if (kvm.kvm_run->io.direction == KVM_EXIT_IO_OUT) {
		uint32_t offset = *(uint32_t *)((uint8_t *)kvm.kvm_run + kvm.kvm_run->io.data_offset);
		const char *filename = (char *)kvm.mem + offset;

		init_fd_map();

		int min_fd;
		for (min_fd = 0; min_fd <= MAX_FD; min_fd++) {
			if (fd_map[min_fd].opening == 0)
				break;
		}

		if (min_fd > MAX_FD) {
			ret = -ENFILE;
		} else {
			int fd = open(filename, O_RDONLY, 0);
			if (fd < 0) {
				ret = -errno;
			} else {
				fd_map[min_fd].real_fd = fd;
				fd_map[min_fd].opening = 1;
				ret = min_fd;
			}
		}
	} else {
		if (ret == UNUSED) {
			kvmkvm_log(LOG_ERROR, "Hypercall open() failed");
			return;
		}
		*(uint32_t*)((uint8_t*)kvm.kvm_run + kvm.kvm_run->io.data_offset) = ret;
		ret = UNUSED;
	}
}

void hp_handle_readwrite(typeof(read) fptr)
{
	static int ret = UNUSED;

	if (kvm.kvm_run->io.direction == KVM_EXIT_IO_OUT) {
		uint32_t offset = *(uint32_t *)((uint8_t *)kvm.kvm_run + kvm.kvm_run->io.data_offset);
		const uint64_t *buf = (uint64_t *)kvm.mem + offset;
		int fd = (int)buf[0];
		uint64_t paddr = buf[1];
		uint64_t n = buf[2];

		if (!(paddr <= (paddr + n) && paddr + (n <= kvm.memsz))) {
			ret = -EACCES;
		} else {
			init_fd_map();
			if (fd < 0 || fd > MAX_FD || fd_map[fd].opening == 0) {
				ret = -EBADF;
			} else {
				ret = fptr(fd_map[fd].real_fd, (uint8_t *)kvm.mem + paddr, n);
				if (ret < 0) {
					ret = -errno;
				}
			}
		}
	}
}

void hp_handle_close(void)
{
	static int ret = UNUSED;

	if (kvm.kvm_run->io.direction == KVM_EXIT_IO_OUT) {
		init_fd_map();
		int fd = *(uint32_t *)((uint8_t *)kvm.kvm_run + kvm.kvm_run->io.data_offset);
		if (fd < 0 || fd > MAX_FD || fd_map[fd].opening == 0) {
			ret = -EBADF;
		} else {
			fd_map[fd].opening = 0;
			ret = close(fd_map[fd].real_fd);
			if (ret < 0) {
				ret = -errno;
			}
		}
	} else {
		if (ret == UNUSED) {
			kvmkvm_log(LOG_ERROR, "hypercall close() failed");
			return;
		}

		*(uint32_t *)((uint8_t *)kvm.kvm_run + kvm.kvm_run->io.data_offset) = ret;
		ret = UNUSED;
	}
}

void hp_handle_lseek(void)
{
	static int ret = UNUSED;
	if (kvm.kvm_run->io.direction == KVM_EXIT_IO_OUT) {
		uint32_t offset = *(uint32_t *)((uint8_t *)kvm.kvm_run + kvm.kvm_run->io.data_offset);
		const uint32_t *buf = (uint32_t *)kvm.kvm_run + offset;
		int fd = buf[0];
		uint32_t off = buf[1];
		int whence = buf[2];

		init_fd_map();
		if (fd < 0 || fd > MAX_FD || fd_map[fd].opening == 0) {
			ret = -EBADF;
		} else {
			ret = lseek(fd_map[fd].real_fd, off, whence);
			if (ret < 0) {
				ret = -errno;
			}
		}
	} else {
		if (ret == UNUSED) {
			kvmkvm_log(LOG_ERROR, "hypercall lseek() failed");
			return;
		}

		*(uint32_t *)((uint8_t *)kvm.kvm_run + kvm.kvm_run->io.data_offset) = ret;
		ret = UNUSED;
	}
}

void hp_handle_exit(void)
{
	int status = *(uint32_t *)((uint8_t *)kvm.kvm_run + kvm.kvm_run->io.data_offset);
	fprintf(stderr, "+++ exited with status %d +++\n", status);
	exit(0);
}

void hp_handle_panic(void)
{
	uint32_t offset = *(uint32_t *)((uint8_t *)kvm.kvm_run + kvm.kvm_run->io.data_offset);
	fprintf(stderr, "[PANIC] %s\n", (uint8_t *)kvm.mem + offset);
	exit(1);

	kvmkvm_log(LOG_FATAL, "KERNEL PANIC");
}

