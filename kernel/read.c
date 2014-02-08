/*
  syscall read
  always read from console
*/
#define NULL 0 /* define NULL since we can't use stdlib*/
#include "drivers/gcd.h"
#include "drivers/device.h"
#include "drivers/polltty.h"
#include "drivers/yams.h"

int syscall_read(int fhandle, void *buffer, int length){
  device_t *dev;
  gcd_t *gcd;
<<<<<<< HEAD
=======
  int actual_read = 0;
>>>>>>> af9188fbe45c17bd674ea543bf9166e948a00333
  int len = 0;

  /*Find the system console (first tty)) */
  dev = device_get(YAMS_TYPECODE_TTY,fhandle);
  
  /* Set gernice char device*/
  gcd = ( gcd_t *) dev->generic_device;
  if(gcd == NULL){ /*if gcd is NULL we have an error*/
    return -1;
  }

<<<<<<< HEAD
  while(length > len){
    gcd->read(gcd, buffer, len);
    len++;
  }
    gcd->write(gcd, buffer, len);
  return len;
=======
  /*  while( gcd->read(gcd, buffer, length) !='\0'){
      actual_read++;
      }*/
  while (len < length){
    len += gcd->read(gcd, buffer,length);
  }
  
  /*debugging remove THIS*/
  len = gcd->write(gcd, buffer,len);
 
  
  return actual_read;

>>>>>>> af9188fbe45c17bd674ea543bf9166e948a00333
  
}
