#include "base/signals.h"

#include <signal.h>
#include <list>
#include <boost/foreach.hpp>
#include <sstream>

#ifdef POSIX
// #define HAVE_GOOGLE_MALLOC_EXTENSION_H
#endif

#if defined(HAVE_GOOGLE_MALLOC_EXTENSION_H)
#include <google/profiler.h>
#endif

#include "base/common.h"
#include "base/once.h"
#include "base/startuplist.h"
#include "base/globalinit.h"

#ifdef POSIX
extern bool RequestMinidump(const std::string &dump_path);
#elif defined(WIN32)
bool RequestMinidump(const std::string &dump_path) {
  return true;
}
#endif

namespace xce {

void InstallSignal(int sig, SignalHandle h) {
#ifdef POSIX
  signal(sig, h);
#elif defined(WIN32)
  // windows不支持自定义 signal
#endif
}

// base: log, rotate
// profile: start / stop
// 

static void SignalProfile(int sig) {
#if defined(HAVE_GOOGLE_MALLOC_EXTENSION_H)
  if (XCE_SIGNAL_PROFILER_START == sig) {
    std::ostringstream ostem;
    ostem << "/tmp/cell_" << getpid() << ".prf";
    ProfilerStart(ostem.str().c_str());
  }
  else if (XCE_SIGNAL_PROFILER_STOP == sig) 
    ProfilerStop();
#endif
}

static void SignalCrash(int sig) {
  ASSERT(XCE_SIGNAL_MINIDUMP == sig);
  if (XCE_SIGNAL_MINIDUMP == sig) {    
    // ::RequestMinidump(".");
  } 
  else if (XCE_SIGNAL_CRASH == sig) {
    *(char *)0 = 'a';
  }
}

static void AddCommonSignal() {
  InstallSignal(XCE_SIGNAL_PROFILER_START, SignalProfile);
  InstallSignal(XCE_SIGNAL_PROFILER_STOP, SignalProfile);

  InstallSignal(XCE_SIGNAL_MINIDUMP, SignalCrash);
  InstallSignal(XCE_SIGNAL_CRASH, SignalCrash);

  // TODO: log rotate
}

GLOBAL_INIT(COMMON_SIGNALS, {
  AddStartup(&AddCommonSignal);
});

}
