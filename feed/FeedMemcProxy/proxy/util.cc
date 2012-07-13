#include <string.h>
#include "util.h"

namespace xce {
namespace feed {


char * GetLineEnd(char * buf, size_t len) {
  char * p = buf + 1; // 首字符肯定不是'\n'

  for(;;) {
    p = (char *)memchr(p, '\n', len - (p - buf));
    if (!p) {
      break;
    }
    if(*(p - 1) == '\r') {
      break;
    }
    ++ p; // p 指向 '\n' 的下一个字符
  }

  return p;
}



int GetAndRemoveCmdEnd(char * buf, size_t len)
{
  char * p = buf+len-1;
  if(len>=5)
  if(memcmp((p-4), "END\r\n", 5)==0)
  {
    return len-5;
  }
  /*int i = 0;
  while(i<len)
  {
    if(*(p+i)=='E')
    {
      if(memcmp((p+i), "END\r\n", 5)==0)
      {
         memset(buf+i,'\0', 5);
         return i;
      }
    }
    i++;
  }*/

  return -1;
}

}
}

