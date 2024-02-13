#ifndef KVMKVM_H
#define KVMKVM_H

#include "kvm.h"

#define MAX_KERNEL_SIZE 0x1F7000
#define UNUSED 0xffdead

extern struct kvm kvm;

int check_iopl(void);

void cleanup(void);
void die(const char *msg);

#endif /* KVMKVM_H */
