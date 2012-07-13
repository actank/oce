#ifndef __USERTALKRECORD_H__
#define __USERTALKRECORD_H__

#include <string>
#include <Ice/Ice.h>
#include "TalkRecord.h"
#include "Singleton.h"
using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::talk::record;

class TalkRecordManagerI : public TalkRecordManager, public Singleton<TalkRecordManagerI>{
public:
	virtual void record(::Ice::Int from , ::Ice::Int to, const string& act, const Ice::Current& = Ice::Current());
};


#endif
