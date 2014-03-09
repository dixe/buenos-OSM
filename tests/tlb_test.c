#include "tests/lib.h"

int main(){
  char *c;
  c  = (char*) malloc(16); // malloc
  int i;
  for(i = 0; i < 16; i++){
    c[i] ='h';
  }

  
  //end string
  c[63] = '\0';
  printf(" here are gome 'h': %s \n",c);

  syscall_halt();
  
  return 0;

}
