/* ftest2, run ftest first to create file [arkimedes]test 
 */
#include "tests/lib.h"

#define BUFFER_SIZE 100

int main (){
  int err = 0;
  int offset = 5; // the byte offset to read from
  int fd;
  char output[BUFFER_SIZE];  
  
   fd = syscall_open("[arkimedes]test");
  if(fd < 2){
    printf("fd was: %d, closing system", fd );
    err = syscall_close(fd);
    syscall_halt();
  }
  err = syscall_read(fd,output,BUFFER_SIZE);
  printf("in file with fd = %d, content is: %s\n", fd, output);  
  // read with ofset
  err = syscall_seek(fd,offset);
  if(err < 0){ // error
    printf("err was: %d, closing system", err );
    err = syscall_close(fd);
    syscall_halt();
  }
  err = syscall_read(fd,output,BUFFER_SIZE);
  printf("in file with fd = %d, content is: %s\n", fd, output);  
  // close file and shut down
  err = syscall_close(fd);
  syscall_halt();
  return 0;
}
