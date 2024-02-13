#ifndef CPU_H
#define CPU_H

#include <linux/kvm.h>

void cpu_setup_segregs(struct kvm_sregs *sregs);

#endif /* CPU_H */
