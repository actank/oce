#ifndef __PING_SENDER_H__
#define __PING_SENDER_H__
#include <string>
#include <vector>
#include <map>
#include <Ice/Ice.h>
#include "TaskManager.h"

namespace xce {
namespace ad {
using namespace std;

class PingSender {
public:
  PingSender(const string& server_name = "localhost", int threshold = 50)
            : server_name_(server_name),
              threshold_(threshold){}; 
  void SetServerInfo(const string& server_name = "dommy@localhost", int threshold = 50);
  bool Send(const string& tag, const int64_t value);

private:
  bool BatchSend(const string& tag, const vector<int64_t>& value);
  string server_name_;
  string node_;
  int threshold_;
  map<string, vector<int64_t> > messages_;
  IceUtil::Mutex mutex_;
  class SendTimer : public MyUtil::Timer {
  public:
    SendTimer(const string& server_name, const string& node, const string& tag,
                const vector<int64_t>& values, int time = 0)
                 : Timer(time), server_name_(server_name), node_(node),
                   tag_(tag), values_(values) {
    }
    virtual void handle();
  private:
    string server_name_;
    string node_;
    string tag_;
    vector<int64_t> values_;
  };
};

}
}

#endif //__PING_SENDER_H__
