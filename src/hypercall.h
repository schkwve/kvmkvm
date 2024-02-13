#ifndef HYPERCALL_H
#define HYPERCALL_H

#define HP_NR_MARK 0x8000

#define NR_HP_open (HP_NR_MARK | 0x00)
#define NR_HP_read (HP_NR_MARK | 0x01)
#define NR_HP_write (HP_NR_MARK | 0x02)
#define NR_HP_close (HP_NR_MARK | 0x03)
#define NR_HP_lseek (HP_NR_MARK | 0x04)
#define NR_HP_exit (HP_NR_MARK | 0x05)
#define NR_HP_panic (HP_NR_MARK | 0x7FFF)

#define MAX_FD 255

struct fd_handle {
	int real_fd;
	int opening;
};

void hp_handle_open(void);

#endif /* HYPERCALL_H */
