#include <unistd.h>
#include <stdio.h>

int putchar(int c)
{
  // write character to stdout
  char * a;
  a = (char *) &c;
  write(1,a,1);
  return c;
}
