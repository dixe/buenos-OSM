/*
 * header for user semaphores
 */
#ifndef BUENOS_PROC_SEMAPHORE
#define BUENOS_PROC_SEMAPHORE

/*Max number of user semaphores, half the number of kernel semaphores*/
#define MAX_USER_SEMAPHORES 64

#include "kernel/semaphore.h"

/*state of semahpore, used or not*/
typedef enum {
  USED,
  FREE
}sem_state;


typedef struct{
  /*holds the kernel semaphore*/
  semaphore_t *ksem;
  /*the semaphore id returned to the user, just the array index */
  int sid;
  sem_state state;
  char *name;
  
} user_sem_t;

// initialize user semaphores
void user_semaphore_init();
/*setup a new user semaphore return the id of it
  return -1 on error*/
int get_sem(char const* name, int value);

/*Check wheather a semahpore with given name exist, return 0 false 1 on true*/
int sem_name_exist(char const *name);

/*given semaphore id, return the semaphore pointer*/
user_sem_t* get_user_sem_sid(int sid);
// return semaphore based on name, return semaphore pointer
user_sem_t* get_user_sem_name(char const* name);

#endif  /*BUENOS_PROC_SEMAPHORE*/
