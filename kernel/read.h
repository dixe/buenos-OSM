/*
 * read systemcall
 */

#ifndef BUENOS_KERNEL_READ_H
#define BUENOS_KERNEL_READ_H

int syscall_read(int fhandle, void *buffer, int length);

#endif /* BUENOS_KERNEL_READ_H */

