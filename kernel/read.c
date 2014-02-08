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
  int len = 0;

  /*Find the system console (first tty)) */
  dev = device_get(YAMS_TYPECODE_TTY,fhandle);
  
  /* Set gernice char device*/
  gcd = ( gcd_t *) dev->generic_device;
  if(gcd == NULL){ /*if gcd is NULL we have an error*/
    return -1;
  }



  len += gcd->read(gcd, buffer, length);


  gcd->read(gcd, buffer, len);
  return len;
}
