#include "tests/lib.h"

#define BUFFER_SIZE 60

int main(void) {
  usr_sem_t *wait0, *wait1, *read_write_lock;
  char line[BUFFER_SIZE];

  /* Open the semaphores. */
  wait0 = syscall_sem_open("wait0", -1);
  wait1 = syscall_sem_open("wait1", -1);
  read_write_lock = syscall_sem_open("rwlock", -1);

  /* Do work before barrier. */
  syscall_sem_p(read_write_lock);
  readline(line, BUFFER_SIZE);
  printf("prog1: You wrote: %s\n", line);
  syscall_sem_v(read_write_lock);

  /* Wait for the other process. */
  syscall_sem_v(wait1); /* DIFFERENT THAN IN prog0 */
  syscall_sem_p(wait0); /* DIFFERENT THAN IN prog0 */

  /* Do work after barrier. */
  puts("prog1: done.\n");

  syscall_exit(0);
  return 0;
}
