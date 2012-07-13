//   LOG(...) an ostream target that can be used to send formatted
// output to a variety of logging targets, such as debugger console, stderr,
// file, or any std::ostream.
//   The severity level passed as the first argument to the the LOGging
// functions is used as a filter, to limit the verbosity of the logging.
//   Static members of LogMessage documented below are used to control the
// verbosity and target of the output.
//   There are several variations on the LOG macro which facilitate logging
// of common error conditions, detailed below.

#ifndef OCE_BASE_LOGGING_H__
#define OCE_BASE_LOGGING_H__

#include <sstream>
#include "base/basictypes.h"


namespace xce {

///////////////////////////////////////////////////////////////////////////////
// ConstantLabel can be used to easily generate string names from constant
// values.  This can be useful for logging descriptive names of error messages.
// Usage:
//   const ConstantLabel LIBRARY_ERRORS[] = {
//     KLABEL(SOME_ERROR),
//     KLABEL(SOME_OTHER_ERROR),
//     ...
//     LASTLABEL
//   }
//
//   int err = LibraryFunc();
//   LOG(LS_ERROR) << "LibraryFunc returned: "
//                 << ErrorName(err, LIBRARY_ERRORS);

struct ConstantLabel { int value; const char * label; };
#define KLABEL(x) { x, #x }
#define TLABEL(x,y) { x, y }
#define LASTLABEL { 0, 0 }

const char * FindLabel(int value, const ConstantLabel entries[]);
std::string ErrorName(int err, const ConstantLabel* err_table);

//////////////////////////////////////////////////////////////////////

// Note that the non-standard LoggingSeverity aliases exist because they are
// still in broad use.  The meanings of the levels are:
//  LS_VERBOSE: This level is for data which we do not want to appear in the
//   normal debug log, but should appear in diagnostic logs.
//  LS_INFO: Chatty level used in debugging for all sorts of things, the default
//   in debug builds.
//  LS_WARNING: Something that may warrant investigation.
//  LS_ERROR: Something that should not have occurred.
enum LoggingSeverity { LS_VERBOSE, LS_INFO, LS_WARNING, LS_ERROR,
                       VERBOSE = LS_VERBOSE,
                       INFO = LS_INFO,
                       WARNING = LS_WARNING,
                       LERROR = LS_ERROR };

// LogErrorContext assists in interpreting the meaning of an error value.
//  ERRCTX_ERRNO: the value was read from global 'errno'
//  ERRCTX_HRESULT: the value is a Windows HRESULT
enum LogErrorContext { ERRCTX_NONE, ERRCTX_ERRNO, ERRCTX_HRESULT };

class LogMessage {
 public:
  LogMessage(const char* file, int line, LoggingSeverity sev,
             LogErrorContext err_ctx = ERRCTX_NONE, int err = 0,
             const char* module = NULL);
  ~LogMessage();

  static inline bool Loggable(LoggingSeverity sev) { return (sev >= min_sev_); }
  std::ostream& stream() { return print_stream_; }

  enum { NO_LOGGING = LS_ERROR + 1 };

  // These are attributes which apply to all logging channels
  //  LogContext: Display the file and line number of the message
  static void LogContext(int min_sev);
  //  LogThreads: Display the thread identifier of the current thread
  static void LogThreads(bool on = true);
  //  LogTimestamps: Display the elapsed time of the program
  static void LogTimestamps(bool on = true);

  // Timestamps begin with program execution, but can be reset with this
  // function for measuring the duration of an activity, or to synchronize
  // timestamps between multiple instances.
  static void ResetTimestamps();

  // These are the available logging channels
  //  Debug: Debug console on Windows, otherwise stderr
  static void LogToDebug(int min_sev);
  static int GetLogToDebug() { return dbg_sev_; }
  //  Stream: Any non-blocking stream interface.  LogMessage takes ownership of
  //   the stream.
  static void LogToStream(std::ostream* stream, int min_sev);
  static int GetLogToStream() { return stream_sev_; }

  // Testing against MinLogSeverity allows code to avoid potentially expensive
  // logging operations by pre-checking the logging level.
  static int GetMinLogSeverity() { return min_sev_; }

  static void SetDiagnosticMode(bool f) { is_diagnostic_mode_ = f; }
  static bool IsDiagnosticMode() { return is_diagnostic_mode_; }

 private:
  // These assist in formatting some parts of the debug output.
  static const char* Describe(LoggingSeverity sev);
  static const char* DescribeFile(const char* file);

  // The ostream that buffers the formatted message before output
  std::ostringstream print_stream_;

  // The severity level of this message
  LoggingSeverity severity_;

  // String data generated in the constructor, that should be appended to
  // the message before output.
  std::string extra_;

  // dbg_sev_ and stream_sev_ are the thresholds for those output targets
  // min_sev_ is the minimum (most verbose) of those levels, and is used
  //  as a short-circuit in the logging macros to identify messages that won't
  //  be logged.
  // ctx_sev_ is the minimum level at which file context is displayed
  static int min_sev_, dbg_sev_, stream_sev_, ctx_sev_;

  // The output stream, if any
  static std::ostream * stream_;

  // Flags for formatting options
  static bool thread_, timestamp_;

  // The timestamp at which logging started.
  static uint32 start_;

  // are we in diagnostic mode (as defined by the app)?
  static bool is_diagnostic_mode_;

  DISALLOW_EVIL_CONSTRUCTORS(LogMessage);
};

//////////////////////////////////////////////////////////////////////
// Logging Helpers
//////////////////////////////////////////////////////////////////////

class LogMultilineState {
public:
  size_t unprintable_count_;
  LogMultilineState() : unprintable_count_(0) { }
};

// When possible, pass optional state variable to track various data across
// multiple calls to LogMultiline.  Otherwise, pass NULL.
void LogMultiline(LoggingSeverity level, const char* label, bool input,
                  const char * data, size_t len, bool hex_mode,
                  LogMultilineState* state);

//////////////////////////////////////////////////////////////////////
// Macros which automatically disable logging when LOGGING == 0
//////////////////////////////////////////////////////////////////////

// If LOGGING is not explicitly defined, default to enabled in debug mode
#ifdef XCE_NO_LOGGING
#define LOGGING 0
#else
#define LOGGING 1
#endif  // NO_LOGGING

#ifndef LOG
#if LOGGING

#define LOG(sev) \
  if (::xce::LogMessage::Loggable(::xce::sev)) \
    ::xce::LogMessage(__FILE__, __LINE__, ::xce::sev).stream()

// The _V version is for when a variable is passed in.  It doesn't do the
// namespace concatination.
#define LOG_V(sev) \
  if (::xce::LogMessage::Loggable(sev)) \
    ::xce::LogMessage(__FILE__, __LINE__, sev).stream()

// The _F version prefixes the message with the current function name.
#ifdef __GNUC__
#define LOG_F(sev) LOG(sev) << __PRETTY_FUNCTION__  << ": "
#else
#define LOG_F(sev) LOG(sev) << __FUNCTION__ << ": "
#endif

// LOG_CHECK_LEVEL can be used as a test before performing expensive or
// sensitive operations whose sole purpose is to output logging data at the
// desired level.
#define LOG_CHECK_LEVEL(sev) \
  ::xce::LogCheckLevel(xce::sev)
#define LOG_CHECK_LEVEL_V(sev) \
  ::xce::LogCheckLevel(sev)
inline bool LogCheckLevel(LoggingSeverity sev) {
  return (LogMessage::GetMinLogSeverity() <= sev);
}

// PLOG and LOG_ERR attempt to provide a string description of an errno derived
// error.  LOG_ERR reads errno directly, so care must be taken to call it before
// errno is reset.
#define PLOG(sev, err) \
  if (::xce::LogMessage::Loggable(xce::sev)) \
    ::xce::LogMessage(__FILE__, __LINE__, ::xce::sev, \
                           xce::ERRCTX_ERRNO, err).stream()
#define LOG_ERR(sev) \
  if (::xce::LogMessage::Loggable(sev)) \
    ::xce::LogMessage(__FILE__, __LINE__, ::xce::sev, ::xce::ERRCTX_ERRNO, errno).stream()

// LOG_GLE(M) attempt to provide a string description of the HRESULT returned
// by GetLastError.  The second variant allows searching of a dll's string
// table for the error description.
#ifdef WIN32
#define LOG_GLE(sev) \
  if (xce::LogMessage::Loggable(xce::sev)) \
    xce::LogMessage(__FILE__, __LINE__, xce::sev, \
                          xce::ERRCTX_HRESULT, GetLastError()).stream()
#define LOG_GLEM(sev, mod) \
  if (xce::LogMessage::Loggable(xce::sev)) \
    xce::LogMessage(__FILE__, __LINE__, xce::sev, \
                          xce::ERRCTX_HRESULT, GetLastError(), mod) \
      .stream()
#endif  // WIN32

// TODO: Add an "assert" wrapper that logs in the same manner.

#else  // !LOGGING

// Hopefully, the compiler will optimize away some of this code.
// Note: syntax of "1 ? (void)0 : LogMessage" was causing errors in g++,
//   converted to "while (false)"
#define LOG(sev) \
  while (false) ::xce::LogMessage(NULL, 0, ::xce::sev).stream()
#define LOG_V(sev) \
  while (false) ::xce::LogMessage(NULL, 0, ::xce::sev).stream()
#define LOG_F(sev) LOG(sev) << __FUNCTION__ << ": "
#define LOG_CHECK_LEVEL(sev) \
  false
#define LOG_CHECK_LEVEL_V(sev) \
  false
#define PLOG(sev, err) \
  while (false) ::xce::LogMessage(NULL, 0, ::xce::sev, \
                                      xce::ERRCTX_ERRNO, 0).stream()
#define LOG_ERR(sev) \
  while (false) ::xce::LogMessage(NULL, 0, ::xce::sev, \
    ::xce::ERRCTX_ERRNO, 0).stream()
#ifdef WIN32
#define LOG_GLE(sev) \
  while (false) ::xce::LogMessage(NULL, 0, ::xce::sev, \
    ::xce::ERRCTX_HRESULT, 0).stream()
#define LOG_GLEM(sev, mod) \
  while (false) ::xce::LogMessage(NULL, 0, ::xce::sev, \
    ::xce::ERRCTX_HRESULT, 0).stream()
#endif  // WIN32

#endif  // !LOGGING
#endif  // LOG

//////////////////////////////////////////////////////////////////////
// #endif

} // xce

#endif  // OCE_BASE_LOGGING_H__
