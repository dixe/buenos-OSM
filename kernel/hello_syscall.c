#inlucde "kernel/hello_syscall.h"
#include "drivers/metadev.h"
#include "lib/libc.h"
#include "fs/vfs.h"

/**
 * Halt the kernel.
 */
void hello_syscall(void)
{

    kprintf("Hello \n");
   
}
