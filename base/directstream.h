#ifndef XCE_BASE_DIRECT_STREAM_H__
#define XCE_BASE_DIRECT_STREAM_H__

#include <ostream>
#include <streambuf>

#ifdef POSIX
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace xce {

template<typename _CharT = char, typename _Traits = std::char_traits<_CharT> >
class DirectBuffer : public std::basic_streambuf<_CharT, _Traits> {
public:
  typedef _CharT                                    char_type;
  typedef _Traits                                   traits_type;
  typedef typename traits_type::int_type            int_type;
  typedef typename traits_type::pos_type            pos_type;
  typedef typename traits_type::off_type            off_type;

  DirectBuffer() : fd_(-1) {}
  ~DirectBuffer() {
    if (-1 != fd_)
      close(fd_);
  }

  bool Open(const char* filename, std::ios::openmode __mode) {
    // TODO: __mode => O_XXX
    fd_ = open(filename, O_WRONLY | O_CREAT| O_APPEND
      , S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    return fd_ != -1;
  }

  virtual std::streamsize xsputn (const char_type *__s, std::streamsize __n) {
    if (-1 != fd_)
      return write(fd_, __s, __n);
    return 0;
  }

private:
  int fd_;
};

class DirectOutStream : public std::ostream {
public:
  explicit DirectOutStream(const char* filename, std::ios::openmode __mode = std::ios::in | std::ios::out) 
    : std::ostream(&buffer_) {
    buffer_.Open(filename, __mode);
  }
  void open(const char* __s, std::ios::openmode __mode = std::ios::in | std::ios::out) {
    buffer_.Open(__s, __mode);
  }
  void close() {}

  DirectBuffer<> buffer_;
};

}  // namespace xce
#endif // POSIX
#endif  // XCE_BASE_DIRECT_STREAM_H__
