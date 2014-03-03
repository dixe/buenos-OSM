#include "tests/lib.h"

#define BUFFER_SIZE 60

int main(void) {
  usr_sem_t *wait0, *wait1, *read_write_lock;
  char line[BUFFER_SIZE];

  // try to create a new semaphore with name wait0, and open one with name wait
  wait0 = syscall_sem_open("wait0", 1);
  if(wait0 == NULL){ // it should be
    printf("prog0n: wait0 is null as it should be \n");
  }
  else{
      printf("prog0n: wait0 is NOT null as it should be \n");
  }

  wait1 = syscall_sem_open("wait", -1);

  if(wait1 == NULL){ // it should be
    printf("prog0n: wait1 is null as it should be \n");
  }
  else{
      printf("prog0n: wait1 is NOT null as it should be \n");
  }

  read_write_lock = syscall_sem_open("rwlock", -1);

  /* Do work before barrier. */
  syscall_sem_p(read_write_lock);
  readline(line, BUFFER_SIZE);
  printf("prog0: You wrote: %s\n", line);
  syscall_sem_v(read_write_lock);

  /* Wait for the other process. */
  syscall_sem_p(wait1); /* DIFFERENT THAN IN prog1 */
  syscall_sem_v(wait0); /* DIFFERENT THAN IN prog1 */

  /* Do work after barrier. */
  puts("prog0: done.\n");

  syscall_exit(0);
  return 0;
}
