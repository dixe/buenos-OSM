#include "tests/lib.h"

int main(void)
{
  char buffer[32] = "Hej dette er en test\n";
  char buffer2[16];

  syscall_read(0,&buffer2,16);
  
  
  syscall_write(0, &buffer,32);
  syscall_write(0, &buffer2,32);
  
  /*halt system after read and write*/
  syscall_halt();
  return 0;
}
