#ifndef XCE_BASE_SIGNALS_H__
#define XCE_BASE_SIGNALS_H__

// ----------------------------------------------------
// log, Rotate
#define XCE_SIGNAL_LOG_OPEN     35
#define XCE_SIGNAL_LOG_ROTATE   36

// profile
#define XCE_SIGNAL_PROFILER_START      37
#define XCE_SIGNAL_PROFILER_STOP       38

// ----------------------------------------------------
// config, 本地缓存的map加载，导出
#define XCE_SIGNAL_CONFIG

// ----------------------------------------------------
// breakpad
#define XCE_SIGNAL_MINIDUMP           45
#define XCE_SIGNAL_CRASH              46

// ----------------------------------------------------
// cache, 加载/Dump
#define XCE_SIGNAL_CACHE_DUMP         50
#define XCE_SIGNAL_CACHE_DUMP_ROTATE  51
#define XCE_SIGNAL_CACHE_RESTORE      52

// ----------------------------------------------------
// for cwf
#define XCE_SIGNAL_CWF_LOAD_TEMPLATE  60

namespace xce {

typedef void (*SignalHandle)(int);

void InstallSignal(int sig, SignalHandle);

}
#endif // XCE_BASE_SIGNALS_H__
