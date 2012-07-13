#include "gtest/gtest.h"

#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/thread.hpp>

#include "LogWrapper.h"
#include "singleton.h"

namespace com {
namespace renren {
namespace messageconfig {

static boost::shared_mutex rw_mutex;
static boost::recursive_mutex rec_mutex;


static void goo() {
  //boost::shared_lock<boost::shared_mutex> read_lock(rw_mutex);
  //boost::unique_lock<boost::shared_mutex> write_lock(rw_mutex);
  boost::lock_guard<boost::recursive_mutex> rec_lock(rec_mutex);
  std::cout << "goo()... thread_id:" << boost::this_thread::get_id() << std::endl;
}

static void foo() {
  //boost::shared_lock<boost::shared_mutex> read_lock(rw_mutex);
  //boost::unique_lock<boost::shared_mutex> write_lock(rw_mutex);
  boost::lock_guard<boost::recursive_mutex> rec_lock(rec_mutex);
  std::cout << "foo()... thread_id:" << boost::this_thread::get_id() << std::endl;
  goo();
}

TEST(rw_lock, test) {
  foo();
}


}}}


