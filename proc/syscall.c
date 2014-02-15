/*
 * System calls.
 *
 * Copyright (C) 2003 Juha Aatrokoski, Timo Lilja,
 *   Leena Salmela, Teemu Takanen, Aleksi Virtanen.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: syscall.c,v 1.3 2004/01/13 11:10:05 ttakanen Exp $
 *
 */
#include "kernel/cswitch.h"
#include "proc/syscall.h"
#include "kernel/halt.h"
#include "kernel/panic.h"
#include "lib/libc.h"
#include "kernel/assert.h"
#include "drivers/gcd.h"
#include "drivers/device.h"
#include "drivers/polltty.h"
#include "drivers/yams.h"

/*
 * Syscalls for buenoes
 */
/*write syscall*/
int syscall_write(int fhandle, const void *buffer, int length){
  device_t *dev;
  gcd_t *gcd;
  int len = 0;
  
  /*
   * we always write to terminal, and we don't need fhandle
   * assign it to itself to avoid compiler warnings ie errors
   */
  fhandle = fhandle;

  /*Find the system console (first tty)) */
  /* Should be FILEHANDLE_STDOUT, which is 1, but when
   * using 1 and not 0 i get kernel assert failed on dev
   */
  dev = device_get(YAMS_TYPECODE_TTY,0);
  if(dev == NULL){
    return -1;
  }

  /* Set gernice char device*/
  gcd = ( gcd_t *) dev->generic_device;
  if(gcd == NULL){
    return -1;
  }
  
  len = gcd->write(gcd, buffer, length);

  return len;
}

/*read syscall*/
int syscall_read(int fhandle, void *buffer, int length){
  device_t *dev;
  gcd_t *gcd;
  int len = 0;

  /*
   * we always read from terminal so we don't need fhandle
   * assign it to itself to avoid compiler warnings ie. errors
   */
  fhandle = fhandle;

  /*Find the system console (first tty)) */
  dev = device_get(YAMS_TYPECODE_TTY,FILEHANDLE_STDIN); 
  if(dev == NULL){
    return -1;
  }

  /* Set gernice char device*/
  gcd = ( gcd_t *) dev->generic_device;
  if(gcd == NULL){
    return -1;
  }

  /*
   * read bytes until we at length or over
   * or until we get a enter, enter in ascii is 13
   */
  while(len <= length && !( *(char*) (buffer+len-1)== 13)){
    /* we read one byte at a type, so we increment len, and
     * store the next byte on the offset of len
     */
    len += gcd->read(gcd, buffer + len, length);

    /* If we don't hit enter, write to
     * the screen what the user typed in
     */
    if(!( *(char*) (buffer+len-1)== 13)){
      // should only write one char
      syscall_write(0,buffer+len-1,length - len);
    }

  }

  return len;
}

/**
 * Handle system calls. Interrupts are enabled when this function is
 * called.
 *
 * @param user_context The userland context (CPU registers as they
 * where when system call instruction was called in userland)
 */
void syscall_handle(context_t *user_context)
{
    /* When a syscall is executed in userland, register a0 contains
     * the number of the syscall. Registers a1, a2 and a3 contain the
     * arguments of the syscall. The userland code expects that after
     * returning from the syscall instruction the return value of the
     * syscall is found in register v0. Before entering this function
     * the userland context has been saved to user_context and after
     * returning from this function the userland context will be
     * restored from user_context.
     */
    switch(user_context->cpu_regs[MIPS_REGISTER_A0]) {
    case SYSCALL_HALT:
      halt_kernel();
      break;
    case SYSCALL_READ:
      user_context->cpu_regs[MIPS_REGISTER_V0]=
	syscall_read(user_context->cpu_regs[MIPS_REGISTER_A1],
		     (void *) user_context->cpu_regs[MIPS_REGISTER_A2],
		     user_context->cpu_regs[MIPS_REGISTER_A3]);
      break;
    case SYSCALL_WRITE:
      user_context->cpu_regs[MIPS_REGISTER_V0]=
	syscall_write(user_context->cpu_regs[MIPS_REGISTER_A1],
		      (void *) user_context->cpu_regs[MIPS_REGISTER_A2],
		      user_context->cpu_regs[MIPS_REGISTER_A3]);
      break;
    default: 
      KERNEL_PANIC("Unhandled system call\n");
    }

    /* Move to next instruction after system call */
    user_context->pc += 4;
}
