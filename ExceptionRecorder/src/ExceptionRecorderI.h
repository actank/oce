#ifndef __SERVER_EXCEPTIONRECORDER_H__
#define __SERVER_EXCEPTIONRECORDER_H__

#include<string>
#include<Ice/Ice.h>    
#include <IceUtil/IceUtil.h>
#include "Singleton.h"           
#include"ExceptionRecorder.h"


namespace xce {
namespace server {

  using std::string;
  using std::cout;
  using std::endl;
  using namespace MyUtil;


  class ExceptionRecorderI: virtual public ExceptionRecorder,
      virtual public MyUtil::Singleton<ExceptionRecorderI> {
    
    virtual bool RecordException(const string& serverName, 
        const string& type, 
        const string& describe,
        const Ice::Current& = Ice::Current());
    

    virtual ExceptionContent getException(Ice::Int time, const Ice::Current& = Ice::Current());


  };
};
};

#endif
