#include "proc/semaphore.h"
#include "kernel/semaphore.h"
#include "lib/libc.h"
#include "kernel/spinlock.h"
#include "kernel/interrupt.h"

/*Array to hold semaphores*/ 
user_sem_t sem_table[MAX_USER_SEMAPHORES];

/*spin lock used when modifying semaphore table*/
spinlock_t sem_table_slock;

void user_semaphore_init(){
  int i;
  /*Set all user semaphores to free*/
  for(i = 0; i<MAX_USER_SEMAPHORES; i++){
    sem_table[i].state = FREE;
  }
  spinlock_reset(&sem_table_slock);
}


int get_sem(char const* name, int value){
  int sid = -1;
  int intr_status, i;
  // disable interrupts and get the lock on semaphore table
  intr_status = _interrupt_disable();
  spinlock_acquire(&sem_table_slock);
    
  /*Find first empty semaphore in semahpore table*/
  for(i = 0; i < MAX_USER_SEMAPHORES; i++){
    if(sem_table[i].state == FREE){
      /*we found a free semaphore*/
      sid = i;
      break;
    }
  }
  
  if(sid == -1){ // full table return -1
    // release lock and reset intertupt before returning
    spinlock_release(&sem_table_slock);
    _interrupt_set_state(intr_status);
    return sid;
  }
  
  // set the semaphore to used
  sem_table[sid].state = USED;
  //set sid
  sem_table[sid].sid = sid;
  // get a kernel semaphore  
  sem_table[sid].ksem = semaphore_create(value);
  // set the name
  stringcopy(sem_table[sid].name, name,strlen(name));

  spinlock_release(&sem_table_slock);
  _interrupt_set_state(intr_status);

  return sid;
}

int sem_name_exist(char const *name){
  int exist = 0;
  int i;
  for(i = 0; i < MAX_USER_SEMAPHORES; i++){
    if(stringcmp(sem_table[i].name,name)==0){ // 0 means equal
      exist = 1;
      break;
    }
  }
  
  return exist;
}

user_sem_t* get_user_sem_sid(int sid){
  // might need a lock here ??
  return &sem_table[sid];
}

user_sem_t* get_user_sem_name(char const* name){
  int sid = -1;
  int i;
  for(i = 0; i < MAX_USER_SEMAPHORES; i++){
    if(stringcmp(sem_table[i].name,name) == 0){ // 0 means equal
      // should be the same as i
      sid = sem_table[i].sid;
      break;
    }
  }
  if(sid == -1){// return null on error
    return NULL;
  }
  
  return &sem_table[sid];
  
}
