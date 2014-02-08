/*
 * write syscall
 */

#ifndef BUENOS_KERNEL_WRITE_H
#define BUENOS_KERNEL_WRITE_H

int syscall_write(int fhandle, void *buffer, int length);

#endif /* BUENOS_KERNEL_WRITE_H */
