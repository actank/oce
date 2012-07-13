#ifndef XCE_BASE_COMMON_H__
#define XCE_BASE_COMMON_H__

#include <boost/static_assert.hpp>

//////////////////////////////////////////////////////////////////////
// General Utilities
//////////////////////////////////////////////////////////////////////

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (static_cast<int>((sizeof(x)/sizeof(x[0]))))
#endif // ARRAY_SIZE

/////////////////////////////////////////////////////////////////////////////
// Assertions
/////////////////////////////////////////////////////////////////////////////

#ifndef XCE_NO_DEBUG

namespace xce {

// Break causes the debugger to stop executing, or the program to abort
void Break();

// LogAssert writes information about an assertion to the log
void LogAssert(const char * function, const char * file, int line, const char * expression);

inline void Assert(bool result, const char * function, const char * file, int line, const char * expression) {
  if (!result) {
    LogAssert(function, file, line, expression);
    Break();
  }
}

size_t CurrentMemoryUsage();
bool GetMemInfo(size_t *total, size_t *free);
}; // namespace xce

#if defined(_MSC_VER) && _MSC_VER < 1300
#define __FUNCTION__ ""
#endif

#ifndef ASSERT
#define ASSERT(x) ::xce::Assert((x),__FUNCTION__,__FILE__,__LINE__,#x)
#endif

#ifndef VERIFY
#define VERIFY(x) ::xce::Assert((x),__FUNCTION__,__FILE__,__LINE__,#x)
#endif

#else // XCE_NO_DEBUG

#ifndef ASSERT
#define ASSERT(x) (void)0
#endif

#ifndef VERIFY
#define VERIFY(x) (void)(x)
#endif

#endif // !ENABLE_DEBUG

#ifndef STATIC_ASSERT
#define STATIC_ASSERT BOOST_STATIC_ASSERT
#endif

namespace xce {

// for asyncall/baseasync :: [Start/Stop]
void Start();
void Stop();


void Sleep(int millisec);

}

#endif // XCE_BASE_COMMON_H__
