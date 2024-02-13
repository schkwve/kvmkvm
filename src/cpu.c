#include <sys/ioctl.h>
#include <linux/kvm.h>

#include "cpu.h"
#include "kvmkvm.h"

void cpu_setup_segregs(struct kvm_sregs *sregs)
{
	struct kvm_segment seg = {
		.base = 0,
		.limit = 0xffffffff,
		.selector = 1 << 3,
		.present = 1,
		.type = 11,
		.dpl = 0,
		.db = 0,
		.s = 1,
		.l = 1,
		.g = 1
	};

	sregs->cs = seg;
	seg.type = 0x03;
	seg.selector = 2 << 3;
	sregs->ds = seg;
	sregs->es = seg;
	sregs->fs = seg;
	sregs->gs = seg;
	sregs->ss = seg;
}