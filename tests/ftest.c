#include "tests/lib.h"

#define BUFFER_SIZE 100

int main (){
  int err = 0;
  int fd;
  char input[BUFFER_SIZE];
  char output[BUFFER_SIZE];  
  char *name = "arkimedes"; // for filecount syscall
  char fname[BUFFER_SIZE]; // for file syscall
  printf("Hello creating file named test2, with size 256\n");
  err = syscall_create("[arkimedes]test2", 256);
  if(err < 0){
    printf("file exist, printing content\n");
    fd = syscall_open("[arkimedes]test2");
    printf("fd is: %d \n",fd);
    if(fd < 2){
      printf("fd was: %d, closing system", fd );
      err = syscall_close(fd);
      syscall_halt();
    }
  }
  else{
    printf("Created file err code is: %d \n", err);
    printf("Type something to go into file\n");
    readline(input, BUFFER_SIZE);
    fd = syscall_open("[arkimedes]test2");
    if(fd < 2){
      printf("fd was: %d, closing system", fd );
      err = syscall_close(fd);
      syscall_halt();
    }
    //write to file
    err = syscall_write(fd,input,BUFFER_SIZE); 
    printf("Wrote to file, err: %d\n", err);
  }


  // test filecount syscall
  printf("Files on arkimedes is %d\n", syscall_filecount(name));
  
  // test file syscall
  err = syscall_file(name, 1, fname);
  printf("File 1 on arkimedes is %s, syscall_file ended with: %d \n", fname,err);

  err = syscall_read(fd,output,BUFFER_SIZE);
  printf("in file with fd = %d, content is: %s\n", fd, output);  
  // close file and shut down
  err = syscall_close(fd);
  syscall_halt();
  return 0;
}
