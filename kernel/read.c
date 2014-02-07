/*
  syscall read
  always read from console
*/
#include "drivers/gcd.h"
#include "drivers/device.h"
#include "drivers/polltty.h"
#include "drivers/yams.h"

int syscall_read(int fhandle, void *buffer, int length){
  device_t *dev;
  gcd_t *gcd;
  int actual_read = 0;
  int len =0;

  /*Find the system console (first tty)) */
  dev = device_get(YAMS_TYPECODE_TTY,0);
  
  /* Set gernice char device*/
  gcd = ( gcd_t *) dev->generic_device;
  if(gcd == NULL){ /*if gcd is NULL we have an error*/
    return -1;
  }

  while(len = gcd->read(gcd, buffer, length) !=EOF){
      actual_read++;
  }

  return actual_read;

  
}
