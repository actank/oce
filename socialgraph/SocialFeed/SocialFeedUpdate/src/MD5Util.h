#ifndef _MD5UTIL_H_
#define _MD5UTIL_H_
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <vector>
#include <openssl/md5.h>
namespace xce {
namespace md5 {

class MD5Util {
public:
	static long stringToMD5(const std::string& source);
	static long stringSeqToMD5(const std::vector<std::string>& source);
};

}
}
#endif
