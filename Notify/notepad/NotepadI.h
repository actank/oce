#ifndef _NOTIFYNGINXI_H_
#define _NOTIFYNGINXI_H_

#include <ctemplate/template.h>
#include <boost/lexical_cast.hpp>

#include "TalkNotepad.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "Notify/util/fcgi_request.h"

using namespace xce::notify;
using namespace MyUtil;

namespace com {
namespace xiaonei {
namespace talk {
namespace notepad {

class AddRequest : public Request {
public:
  AddRequest(FCGX_Request* r) : Request(r) {}

  virtual bool response();
};

class RemoveRequest : public Request {
public:
  RemoveRequest(FCGX_Request* r) : Request(r) {}

  virtual bool response();
};

class GetRequest : public Request {
public:
  GetRequest(FCGX_Request* r) : Request(r) {}

  virtual bool response();
};

class NotepadManagerI : public NotepadManager, public MyUtil::Singleton<NotepadManagerI> {
public:
  NotepadManagerI();

  virtual bool addNote(Ice::Long id, Ice::Int owner, const std::string & title, 
      const std::string & content, const Ice::Current& = Ice::Current()) {
    return false;
  }

  virtual bool removeNote(Ice::Long id, Ice::Int owner, const Ice::Current& = Ice::Current()) {
    return false;
  }

  Ice::Long CreateNoteId();

private:
  IceUtil::Mutex mutex_;
  Ice::Long cur_index_; 
  Ice::Long top_index_;
  int count_;
};

class AddNoteTask: public Task {
public:
  AddNoteTask(Ice::Long id, Ice::Int owner, bool check, const string & subject, const string & content)
    : id_(id)
    , owner_(owner)
    , check_owner_(check)
    , subject_(subject)
    , content_(content){
  }

  virtual void handle();

private:
  Ice::Long id_;
  Ice::Int  owner_;
  bool check_owner_;
  string    subject_;
  string    content_;
};

class RemoveNoteTask: public Task {
public:
  RemoveNoteTask(Ice::Long id, Ice::Int owner)
    : id_(id)
    , owner_(owner) {
  }

  virtual void handle();
private:
  Ice::Long id_;
  Ice::Int  owner_;
};

}
}
}
}

#endif // _NOTIFYNGINXI_H_

