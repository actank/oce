#include <string.h>
#include "util.h"

namespace xce {
namespace feed {

// 读出一个command line, 返回值指向'\r\n' 的'\n'字符
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

}
}

