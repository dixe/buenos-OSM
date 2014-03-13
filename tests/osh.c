/*
 * OSM shell.
 */

#include "tests/lib.h"

#define BUFFER_SIZE 100
#define PATH_LENGTH 256


typedef int (*cmd_fun_t)(int, char**);

typedef struct {
  char* name;
  cmd_fun_t func;
  char *desc;
} cmd_t;

int cmd_echo(int, char**);
int cmd_show(int, char**);
int cmd_read(int, char**);
int cmd_help();
int cmd_exit();
int cmd_rm(int, char**);
int cmd_cp(int, char**);
int cmd_cmp(int, char**);

cmd_t commands[] =
  { {"echo", cmd_echo, "print the arguments to the screen"},
    {"show", cmd_show, "print the contents of the given file to the screen"},
    {"read", cmd_read, "read a line from standard in and write it to a new file"},
    {"help", cmd_help, "show this help message"},
    {"exit", cmd_exit, "exit the terminal"},
    {"rm", cmd_rm, "delete file given as argument"},
    {"cp", cmd_cp, "copy contents from file in arg1 to file in arg2"},
    {"cmp", cmd_cmp, "compare contents of arg1 and 2, return 0 if equal"}
    
  };

#define N_COMMANDS sizeof(commands) / sizeof(cmd_t)

void print_prompt(int last_retval) {
  printf("%d> ", last_retval);
}

/* Note that tokenize(cmdline, argv) modifies cmdline by inserting NUL
   characters. */
int tokenize(char* cmdline, char** argv) {
  int argc = 0;
  int inword=0;
  char *s, *p;
  for (s = cmdline, p = cmdline; *s; s++) {
    if (*s == ' ' && inword) {
      inword=0;
      argv[argc++]=p;
      *s = '\0';
    } else if (*s != ' ' && !inword) {
      inword=1;
      p=s;
    }
  }
  if (inword) {
    argv[argc++]=p;
  }
  return argc;
}

int run_program(char* prog) {
  return syscall_join(syscall_exec(prog));
}

int run_command(char* cmdline) {
  char* argv[BUFFER_SIZE];
  int argc = tokenize(cmdline, argv);
  unsigned int i;
  if (argc == 0) {
    return 0;
  }
  for (i = 0; i < N_COMMANDS; i++) {
    if (strcmp(argv[0], commands[i].name) == 0) {
      return commands[i].func(argc, argv);
    }
  }
  return run_program(cmdline);
}

void help() {
  printf("Welcome to the Buenos Shell!\n");
  printf("The following commands are available:\n");
  unsigned int i;
  for (i = 0; i < N_COMMANDS; i++) {
    printf("  %s: %s\n", commands[i].name, commands[i].desc);
  }
}

int main(void) {
  char cmdline[BUFFER_SIZE];
  int ret = 0;
  help();
  while (1) {
    print_prompt(ret);
    readline(cmdline, BUFFER_SIZE);    
    run_command(cmdline);
  }
  syscall_halt();
  return 0;
}



int cmd_echo(int argc, char** argv) {
  int i;
  for (i = 1; i < argc; i++) {
    printf("%s ", argv[i]);
  }
  puts("\n");
  return 0;
}

int cmd_show(int argc, char** argv) {
  if (argc != 2) {
    printf("Usage: show <file>\n");
    return 1;
  }
  int fd;
  if ((fd=syscall_open(argv[1])) < 0) {
    printf("Could not open %s.  Reason: %d\n", argv[1], fd);
    return 1;
  }

  int rd;
  char buffer[BUFFER_SIZE];
  while ((rd = syscall_read(fd, buffer, BUFFER_SIZE))) {
    int wr=0, thiswr;
    while (wr < rd) {
      if ((thiswr = syscall_write(1, buffer+wr, rd-wr)) <= 0) {
        printf("\nCall to syscall_write() failed.  Reason: %d.\n", wr);
        syscall_close(fd);
        return 1;
      }
      wr += thiswr;
    }
  }
  if (rd < 0) {
    printf("\nCall to syscall_read() failed.  Reason: %d.\n", rd);
    syscall_close(fd);
    return 1;
  } else {
    syscall_close(fd);
    return 0;
  }
}

int cmd_read(int argc, char** argv) {
  if (argc < 2) {
    printf("Usage: read <file>\n");
    return 1;
  }
  char text[BUFFER_SIZE];
  int count, ret, fd, wr;
  count = readline(text, BUFFER_SIZE - 1) + 1;
  text[count - 1] = '\n';
  text[count] = '\0';
  if ((ret=syscall_create(argv[1], count)) < 0) {
    printf("Could not create %s with initial size %d.  Reason: %d\n", argv[1], count, ret);
    return 1;
  }
  if ((fd=syscall_open(argv[1])) < 0) {
    printf("Could not open %s.  Reason: %d\n", argv[1], fd);
    return 1;
  }
  if ((wr=syscall_write(fd, text, count)) <= 0) {
    printf("\nCall to syscall_write() failed.  Reason: %d.\n", wr);
    syscall_close(fd);
    return 1;
  }
  syscall_close(fd);
  return 0;
}

int cmd_help() {
  help();
  return 0;
}

int cmd_exit() {
  // halt the process
  syscall_exit(0);
  return 0;
}

int cmd_rm(int argc, char** argv) {
  if (argc < 2) {
    printf("Usage: rm <file>\n");
    return 1;
  }
  int ret;
  if((ret = syscall_delete(argv[1])) < 0 ){
    printf("Could not delete %s.  Reason: %d\n", argv[1],ret);
    return 1;
  }
  return 0;
}


int cmd_cmp(int argc, char** argv) {
  if (argc < 3) {
    printf("Usage: rm <file> <file>\n");
    return 1;
  }
  int fd1, fd2;
  int count, equal, err1, err2, ret;
  char text1[1]; // holds 1 byte
  char text2[1]; // holds 1 byte
  if ((fd1=syscall_open(argv[1])) < 0) {
    printf("Could not open %s.  Reason: %d\n", argv[1], fd1);
    return 1;
  }
  if ((fd2=syscall_open(argv[2])) < 0) {
    printf("Could not open %s.  Reason: %d\n", argv[2], fd2);
    return 1;
  }

  count = 0;
  equal = 1;// start out assuming they are equal
  while(equal){
    // read one byte from each
    syscall_read(fd1, text1,1);
    syscall_read(fd2, text2,1);

    if(!(text1[0] == text2[0])){
      equal = 0;
      break;      
    }
    count++;
    // set the next read position
    err1 = syscall_seek(fd1,count);
    err2 = syscall_seek(fd2,count);
    //they are not equal 
    if((err1 < 0) || (err2 < 0)){
      break;
    }
    
  }
  ret = 0;
  if (!equal){
    printf("File differ first at %d, file 1 has %s, file 2 has %s\n", count, text1[0], text2[0]);
    ret = 1;
  }

  syscall_close(fd1);
  syscall_close(fd2);
  return ret;
}

int cmd_cp(int argc, char** argv) {
  if (argc < 4) {
    printf("Usage: rm <file> <newfile> <newfiel size>\n");
    return 1;
  }

  int fd1, fd2, ret, done, count, err1, err2;
  char text[1];
  //create new file with name argv[2] and size argv[3]
  if ((ret = syscall_create(argv[2],atoi(argv[3]))) < 0){
    printf("Could not create %s with initial size %d.  Reason: %d\n", argv[2],atoi(argv[3]), ret);
    return 1;
  }
  
  if ((fd1=syscall_open(argv[1])) < 0) {
    printf("Could not open %s.  Reason: %d\n", argv[1], fd1);
    return 1;
  }
  
  if ((fd2 = syscall_open(argv[2])) < 0) {
    printf("Could not open %s.  Reason: %d\n", argv[2], fd2);
    return 1;
  }
  
  // both files are created and open
  
  count = 0;
  done = 0;// start out assuming they are equal
  while(!done && count < atoi(argv[3])){
    // read one byte from file1
    syscall_read(fd1, text,1);
    // write it to file2
    syscall_write(fd2, text,1);
    
    count++;
    // set the next read and write position
    err1 = syscall_seek(fd1,count);
    err2 = syscall_seek(fd2,count);
    //they are not equal 
    if((err1 < 0) || (err2 < 0)){
      done = 1;
    }    
    
    // if we read a '\0' we stop
    if(text[0] == '\0'){
      done = 1;
    }
  }

  return 0;
}
