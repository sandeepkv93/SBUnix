#include <stdio.h>

int putstr(const char *s)
{
  for( ; *s; ++s) if (putchar(*s) != *s) return EOF;
  return 0;
}

int puts(const char *s)
{
  for( ; *s; ++s) if (putchar(*s) != *s) return EOF;
  return (putchar('\n') == '\n') ? 0 : EOF;
}
