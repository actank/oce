#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <fstream>

#ifdef WIN32
#include <windows.h>
#endif  // WIN32

#include "base/common.h"
#include "base/logging.h"

#ifndef XCE_DISABLE_POOL
#include "base/asyncall.h"
#include "base/baseasync.h"
#endif

#ifdef POSIX
#include <unistd.h>
//#define HAVE_GOOGLE_MALLOC_EXTENSION_H
#endif

#ifdef HAVE_GOOGLE_MALLOC_EXTENSION_H
#include <google/malloc_extension.h>
#endif

//////////////////////////////////////////////////////////////////////
// Assertions
//////////////////////////////////////////////////////////////////////

namespace xce {

void OutputTrace() {
  // TODO:
}

void Break() {
#ifdef WIN32
  ::DebugBreak();
#else // !WIN32

#if _DEBUG_HAVE_BACKTRACE
  OutputTrace();
#endif

  // 服务不能因为一个断言而崩溃
  // abort();
#endif // !WIN32
}

void LogAssert(const char * function, const char * file, int line, const char * expression) {
  // TODO - if we put hooks in here, we can do a lot fancier logging
  // fprintf(stderr, "%s(%d): %s @ %s\n", file, line, expression, function);
  LOG(WARNING) << file << "(" << line << "): " << expression
    << " @ " << function;
}

//////////////////////////////////////////////////////////////////////
// 异步执行线程池
//////////////////////////////////////////////////////////////////////

#ifndef XCE_DISABLE_POOL
static std::auto_ptr<boost::asio::io_service::work> work_;
// TODO: change asyncall.h like baseasync.h
// static std::auto_ptr<boost::asio::io_service::work> work_base_;

void Start() {
//   work_base_.reset(
//     new boost::asio::io_service::work(detail::Async::GetService())
//     );
  detail::Async::Start();

  work_.reset(
    new boost::asio::io_service::work(AsyncRunner::GetService())
    );
  AsyncRunner::instance().Start();
}

void Stop() {
  work_.reset();
  AsyncRunner::instance().Stop();

  // work_base_.reset();
  detail::Async::Stop();
}

#else

void Start() {}
void Stop() {}

#endif

#ifdef HAVE_GOOGLE_MALLOC_EXTENSION_H
size_t CurrentMemoryUsage() {
  size_t result;
  if (MallocExtension::instance()->GetNumericProperty(
        "generic.current_allocated_bytes"
        , &result)) {
    return result;
  } else {
    return 0;
  }
}
#else
size_t CurrentMemoryUsage() {
  return 0;
}
#endif

void Sleep(int millisec) {
#ifdef POSIX
  usleep(millisec * 1000);
#else
  ::Sleep(millisec * 1000);
#endif
}
// k unit
bool GetMemInfo(size_t *total, size_t *free) {

  std::ifstream is("/proc/meminfo");
  if (!is.is_open())
    return false;
  char buf[1024];
  is.getline(buf, 1024);
  if (total)
    *total = atoi(buf+12);

  is.getline(buf, 1024);
  int free0 = atoi(buf+12);

  is.getline(buf, 1024);
  int buffer = atoi(buf+12);

  is.getline(buf, 1024);
  int cached = atoi(buf+12);
  is.close();

  if (free)
    *free = free0 + buffer + cached;
  return true;
}

} // namespace xce
