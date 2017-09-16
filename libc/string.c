void charcpy(char * dest, char * src, int char_num)
{
        while(char_num--)
        {
                *dest++ = *src++;
        }
}

void longcpy(long * dest, long * src, int long_num)
{
        while(long_num--)
        {
                *dest++ = *src++;
        }
}

void memcpy(void * dest, void * src, int size)
{
  int char_num = size % sizeof(long);
  int long_num = size / sizeof(long);
  int char_start = 0;
  if(long_num)
  {
     longcpy((long *)dest,(long*)src,long_num);
     char_start = size - char_num; 
  }
  char_start = size - char_num;
  if(char_num)
  {
     charcpy((char*)(dest+char_start),(char*)(src+char_start),char_num);
  }
}

int
strcmp(const char* s1, const char* s2)
{
    while (*s1 == *s2 && *s1 != '\0') {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

int
strlen(const char* s1)
{
    int len = 0;
    while (*s1++ != '\0') {
        len++;
    }
    return len;
}

char*
strcpy(char* dest, const char* src)
{
    char* temp;
    temp = dest;
    while ((*temp++ = *src++) != '\0')
        ;
    return dest;
}

char*
strncpy(char* dest, const char* src, int n)
{
    char* temp;
    temp = dest;
    while (n && (*temp++ = *src++) != '\0') {
        n--;
    }
    return dest;
}

char*
strcat(char* dest, const char* src)
{
    char* temp = dest;
    while (*temp != '\0') {
        temp++;
    }
    while ((*temp++ = *src++) != '\0')
        ;
    *temp = '\0';
    return dest;
}
