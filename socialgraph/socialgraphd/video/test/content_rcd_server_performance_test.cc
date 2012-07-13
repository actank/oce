#include <fstream>
#include <vector>

#include "boost/foreach.hpp"

#include "base/logrotate.h"
#include "base/logging.h"
#include "base/threadpool.h"
#include "base/ptime.h"
#include "base/stringdigest.h"

#include "OceCxxAdapter/src/ContentRcdAdapter.h"
#include "socialgraph/socialgraphutil/algostuff.h"


namespace xce {
namespace socialgraph {

class ContentRcdMessage : public xce::Message {
 public:
  ContentRcdMessage(int id,const std::string u) : id_(id), url_(u) {

  }
  void Run() {
    while (true) {
      ContentRcd cr;
      cr.md5 = MD5(url_);
      cr.desc = url_;
      cr.lisc = 10;
      try {
        ContentRcdAdapter::instance().GetVideoRcd(id_, cr);
      } catch (Ice::Exception& e) {
        LOG(LERROR) << e.what();
      }
      break;
    }
  }
 private:
  int id_;
  std::string url_;
};

}}


void Main(const std::string& arg) {
  using namespace xce::socialgraph;
  PTIME(pt, "main", true, 0);
  std::vector<std::string> lines;
  xce::GetTextFileLines(arg, lines);
  LOG(INFO) << "lines.size():" << lines.size();
  std::vector<xce::Message*> messages;
  for (size_t i = 0; i < lines.size(); ++i) {
    messages.push_back(new ContentRcdMessage(i, lines[i]));
  }
  LOG(INFO) << "messages.size():" << messages.size();
  xce::ThreadPool thread_pool(20, 40);
  for (size_t i = 0; i < messages.size(); ++i) {
    thread_pool.Post(messages[i]);
  }
  sleep(2);
  thread_pool.Stop();
  thread_pool.Join();  
}


int main(int argc, char **argv) {
  LOG(INFO) << "argc:" << argc;
  if (argc < 3) {
    LOG(LERROR) << "argv error";
    return 0;
  }
  xce::LogRotate::instance().Start(argv[2], xce::LS_INFO);
  Main(argv[1]);
  xce::LogRotate::instance().Stop();
  return 0;
}

