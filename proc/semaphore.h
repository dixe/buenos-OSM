/*
 * header for user semaphores
 */
#ifndef BUENOS_PROC_SEMAPHORE
#define BUENOS_PROC_SEMAPHORE

/*Max number of user semaphores, half the number of kernel semaphores*/
#define MAX_USER_SEMAPHORES 64
// we don't need name longer then 20 chars
#define SEMAPHORE_NAME_LENGTH 20

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
  char name[SEMAPHORE_NAME_LENGTH];
  
} user_sem_t;

// initialize user semaphores
void user_semaphore_init();

//open a user semaphore given name and value
user_sem_t *user_sem_open(char const *name, int value);

/*setup a new user semaphore return the id of it
  return -1 on error*/
int get_sem(char const* name, int value);

/*given semaphore id, return the semaphore pointer*/
user_sem_t* get_user_sem_sid(int sid);

// return semaphore based on name, return semaphore pointer
user_sem_t* get_user_sem_name(char const* name);

/*Check wheather a semahpore with given name exist, return 0 false 1 on true*/
int sem_name_exist(char const *name);

//user semaphore P function
int user_sem_p(user_sem_t* handle);

//user semaphore V function
int user_sem_v(user_sem_t* handle);
int user_sem_destroy(user_sem_t* handle);
#endif  /*BUENOS_PROC_SEMAPHORE*/


