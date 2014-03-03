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

user_sem_t *user_sem_open(char const *name, int value){

  int sid ;
  
  if(value >= 0){
    if(sem_name_exist(name)){ // semaphore with given name exist+
      return NULL;
    }    

    // try to get a new semaphore
    sid = get_sem(name, value);
    if(sid == -1){// no more free semaphores, return NULL to indicate error
      return NULL;
    }
    /* if we are here, we know sid is valid, and has been set to a new
     * user semaphore, so we use the
     */    
    return get_user_sem_sid(sid);

  }
  else{ // value negative
    //return semaphore with name, return null if not found
    return get_user_sem_name(name);
    
  }
  //if  we are here, something is wrong and we indicate error
  return NULL;
}

//Check if semaphore with given name exist, return 1 if one exist and 0 if not
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

// try to get a kernel semaphore, return sid i.e. table index, -1 on error
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
  stringcopy(sem_table[sid].name, name, SEMAPHORE_NAME_LENGTH);

  spinlock_release(&sem_table_slock);
  _interrupt_set_state(intr_status);

  return sid;
}

// return user semaphore given sid
user_sem_t* get_user_sem_sid(int sid){
  return &sem_table[sid];
}

// return user semaphore given name, null if none exist
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

// return 0 success -1 on error
int user_sem_p(user_sem_t* handle){
  //call kernel semaphore_P on the kernel semaphore in handle
  if (handle == NULL){
    return -1;
  }

  semaphore_P(handle->ksem);
  
  return 0;
}

// return 0 success -1 on error
int user_sem_v(user_sem_t* handle){
  if (handle == NULL){
    return -1;
  }
  //call kernel semaphore_V on the kernel semaphore in handle
  semaphore_V(handle->ksem);
  return 0;
}

int user_sem_destroy(user_sem_t* handle){
   if (handle == NULL){
    return -1;
  }
  //call kernel semaphore_destroy
  semaphore_destroy(handle->ksem);

  return 0;
}
