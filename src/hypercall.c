#include <stdint.h>
#include <errno.h>
#include <fcntl.h>

#include "hypercall.h"
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
	static int ret = 0;

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
		*(uint32_t *)((uint8_t *)kvm.kvm_run + kvm.kvm_run->io.data_offset) = ret;
	}
}
