///
/// @file common.h
/// @brief include common headers and macros
///
#ifndef _FRIENDOFFRIENDSEARCH_LIBRARY_COMMON_H
#define _FRIENDOFFRIENDSEARCH_LIBRARY_COMMON_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <cstdio>
#include <map>
#include <string>
#include <vector>

#include <LogWrapper.h>
#include <FriendOfFriendCache.h>

#define LOGP(fmt, arg...) do { \
  fprintf(stderr, __FILE__ "(%d): " fmt "\n", __LINE__, ##arg); \
} while (0);

#define NAMESPACE_HEADER \
  namespace com { \
  namespace renren{ \
  namespace search2{ \
  namespace friendoffriend {

#define NAMESPACE_FOOTER \
  } \
  } \
  } \
  } 

#endif // _FRIENDOFFRIENDSEARCH_LIBRARY_COMMON_H
