#ifndef KVMKVM_H
#define KVMKVM_H

#include "kvm.h"

extern struct kvm kvm;

void cleanup(void);

void die(const char *msg);

#endif /* KVMKVM_H */
