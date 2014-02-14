#include "tests/lib.h"
#include "lib/libc.h"

int main(void)
{
  char buffer[32] = "Write something: ";
  char buffer2[32];
  int len;
  
  /*Write the contnt of buffer*/
  syscall_write(0, buffer,31);
  
  /*Read one byte from the screen*/
  len = syscall_read(0, buffer2,31); 
  buffer2[len] = '\0';

  /*echo what the user wrote*/
  char buffer3[32] = "\nYou said: ";
  syscall_write(0,buffer3,31);

  /*Write what the user wrote*/
  syscall_write(0, buffer2,31);   

  char buffer4[32] = "\n";
  syscall_write(0,buffer4,31);
  
 /*halt system after read and write*/
  syscall_halt();
  return 0;
}
