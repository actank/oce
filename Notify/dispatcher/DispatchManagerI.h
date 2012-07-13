/*
 * DispatchManagerI.h
 *
 *  Created on: Dec 9, 2009
 *      Author: antonio
 */   

#ifndef DISPATCHMANAGERI_H_
#define DISPATCHMANAGERI_H_

#include "Notify.h"
#include "Singleton.h"
#include "ServiceI.h"


namespace xce
{
namespace notify
{

const int ID_SEQ_STEP = 1000;
const string DB_INSTANCE = "notify";
const string TAB_NOTIFY_ID_SEQ = "id_seq";

  
class DispatchManagerI : public NotifyDispatcher,public MyUtil::Singleton<DispatchManagerI>{

public:
  DispatchManagerI() : _curIndex(-1), _topIndex(-1), _count(0) {}
  virtual void dispatch(const MyUtil::Str2StrMap& props, const Ice::Current& = Ice::Current());
  virtual void deliverWebpager(const MyUtil::Str2StrMap& props, const Ice::Current& = Ice::Current());
  virtual bool reloadSchema(const Ice::Current& = Ice::Current());

  Ice::Long createNotifyId();
private:
  Ice::Long GetServreIndex(Ice::Long nid, const MyUtil::Str2StrMap& props);

  Ice::Long _curIndex;
  Ice::Long _topIndex;
  int _count;
  IceUtil::Mutex _mutex;
};

}
}

#endif /* DISPATCHMANAGERI_H_ */
