#ifndef XCE_BASE_MKDIRS_H_
#define XCE_BASE_MKDIRS_H_

#include <sys/stat.h>

namespace xce {
#ifdef POSIX

int mkdirs(const char *pathname, mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

#else

int mkdirs(const char *pathname, int mode = 0);

#endif
}
#endif // XCE_BASE_MKDIRS_H_
