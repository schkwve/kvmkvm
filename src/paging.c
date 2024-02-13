#include <stdint.h>
#include <linux/kvm.h>

#include "paging.h"
#include "kvmkvm.h"

void paging_create_pt(void *mem, struct kvm_sregs *sregs)
{
	uint64_t pml4_addr = 0x1000;
	uint64_t *pml4 = (void *)((uint64_t)mem + pml4_addr);

	uint64_t pdpt_addr = 0x2000;
	uint64_t *pdpt = (void *)((uint64_t)mem + pdpt_addr);

	uint64_t pd_addr = 0x3000;
	uint64_t *pd = (void *)((uint64_t)mem + pd_addr);

	pml4[0] = PDE_ACCESS | PDE_RW | pdpt_addr;
	pdpt[0] = PDE_ACCESS | PDE_RW | pd_addr;
	pd[0] = PDE_ACCESS | PDE_RW | PDE_PRESENT;

	sregs->cr3 = pml4_addr;
	sregs->cr4 = CR4_PAE;
	sregs->cr4 |= CR4_OSFXSR | CR4_OSXMMEXCPT;
	sregs->cr0 = CR0_PE | CR0_MP | CR0_ET | CR0_NE | CR0_WP | CR0_AM | CR0_PG;
	sregs->efer = EFER_LME | EFER_LMA;
}