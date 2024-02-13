#ifndef KVMKVM_H
#define KVMKVM_H

#include "kvm.h"

#define UNUSED 0xffdead

extern struct kvm kvm;

void cleanup(void);

void die(const char *msg);

#endif /* KVMKVM_H */
