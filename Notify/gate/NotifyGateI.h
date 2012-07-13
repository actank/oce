#ifndef _NOTIFYNGINXI_H_
#define _NOTIFYNGINXI_H_

#include <ctemplate/template.h>
#include <boost/lexical_cast.hpp>

#include "Notify.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "NotifyIndexAdapter.h"
#include "Notify/util/fcgi_request.h"


namespace xce {
namespace notify {


using namespace Ice;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace ctemplate;

const string NOTIFY_FCGI_HOST = "notify.renren.com";
  
class GetRequest : public Request{
public:
  GetRequest(FCGX_Request* r) : Request(r) {}

  virtual bool response();

};



class RemoveRequest : public Request{
public:
  RemoveRequest(FCGX_Request* r) : Request(r) {}

  virtual bool response();
};

class DispatchRequest : public Request{
public:
  DispatchRequest(FCGX_Request* r) : Request(r) {}

  virtual bool response();
};

class UnreadCountRequest: public Request {
public:
  UnreadCountRequest(FCGX_Request* r) : Request(r) {}
  virtual bool response();
};

class QuitChatRequest: public Request {
public:
  QuitChatRequest(FCGX_Request* r) : Request(r) {}

  virtual bool response();
};

class NotifyGateI : public NotifyGate, public MyUtil::Singleton<NotifyGateI> {
public:
  NotifyGateI();
  virtual bool reloadSchema(const Ice::Current& =  Ice::Current());

  void instantNotify(const NotifyContentPtr& content,
      const MyUtil::IntSeq& targets, const Ice::Current& = Ice::Current());

  virtual void deliverWebpager(const NotifyContentPtr& r,
      const MyUtil::IntSeq& target, const Ice::Current& = Ice::Current());

  virtual StrSeq getNotifySeq(Ice::Int, Ice::Int, Ice::Long, Ice::Int, const Ice::Current& =  Ice::Current());
  virtual StrSeq getReplySeq(Ice::Int, Ice::Int, Ice::Long, Ice::Int, const Ice::Current& =  Ice::Current());
  virtual StrSeq getByType(Ice::Int, Ice::Int, const MyUtil::IntSeq&, Ice::Long, Ice::Int, const Ice::Current& =  Ice::Current());

  virtual StrSeq getNotifySeq2(Ice::Int, Ice::Int, Ice::Int, Ice::Int, const Ice::Current& =  Ice::Current());
  virtual StrSeq getReplySeq2(Ice::Int, Ice::Int, Ice::Int, Ice::Int, const Ice::Current& =  Ice::Current());
  virtual StrSeq getByType2(Ice::Int, Ice::Int, const MyUtil::IntSeq&, Ice::Int, Ice::Int, const Ice::Current& =  Ice::Current());

  //Ice直接访问，有合并的
  virtual StrSeq getNotifySeqX(Ice::Int, Ice::Int, Ice::Long, Ice::Int, const Ice::Current& =  Ice::Current());
  virtual StrSeq getReplySeqX(Ice::Int, Ice::Int, Ice::Long, Ice::Int, const Ice::Current& =  Ice::Current());

  void BodySeq2StrSeq(const NotifyBodySeq & n, Ice::Int view, StrSeq & s,vector<Ice::Long> &fail_ids,bool is_kaixin = false);
  void BodySeq2StrSeqMerge(const NotifyBodySeq & n, Ice::Int view, StrSeq & s,vector<Ice::Long> &fail_ids,bool is_kaixin = false);

  void BodySeq2StrSeqX(const NotifyBodySeq & n, Ice::Int view, StrSeq & s,vector<Ice::Long> &fail_ids,bool is_kaixin = false);

  virtual void QuitChat(::Ice::Int, const GroupIndexSeq&, const ::Ice::Current& = ::Ice::Current());

  template<typename T>
  static string Print(const T & container) { //将容器内容打印成字符串
    ostringstream ios;
    bool first_item = true;
    int i = 0;
    for (typename T::const_iterator itr = container.begin(); itr
        != container.end(); ++itr,++i) {
      if (first_item == true) {
        first_item = false;
      } else {
        ios << "_";
      }
        ios << *itr;
    }
    return ios.str();
  }
private:
  //std::string _fcgi_socket;

};

class NotifyTargetHandler{
public:
	static string ToString(const NotifyTargetPtr & ntarget);
};

}
}

#endif // _NOTIFYNGINXI_H_

