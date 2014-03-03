/* g3 test 1, based on barrier test
 * negative test
 */

#include "tests/lib.h"

#define VOLUME "[arkimedes]"

int main(void) {
  usr_sem_t *wait0, *wait1, *read_write_lock;
  uint32_t prog0n, prog1n;
  int ret0, ret1;

  /* Create the semaphores. */
  wait0 = syscall_sem_open("wait0", 0);
  wait1 = syscall_sem_open("wait1", 0);
  read_write_lock = syscall_sem_open("rwlock", 1);
    
  /* Run the childs. */
  prog0n = syscall_exec(VOLUME "prog0n");
  // join prog0n and start prog1n
  ret0 = syscall_join(prog0n);
  prog1n = syscall_exec(VOLUME "prog1n");

  /* Wait for them to finish. */
  ret1 = syscall_join(prog1n);
  printf("Childs joined with return values %d and %d.\n", ret0, ret1);

  syscall_halt();
  //to keep compiler happy :)
  wait0=wait0;
  wait1 =wait1;
  read_write_lock= read_write_lock;
  return 0;
}
