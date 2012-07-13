#ifndef _NOTICEPASSAGE_H_
#define _NOTICEPASSAGE_H_



#include "Singleton.h"
#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Shared.h>
#include "TalkProxy.h"
#include "ServiceI.h"
#include "TalkCommon.h"
#include "TalkUtil/src/pugixml.hpp"
#include "Pipe.h"



namespace com
{
namespace xiaonei
{
namespace talk
{

using namespace MyUtil;
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace pugi;



class NoticePipe: 	public Pipe {
public:
	NoticePipe(const string& name, int index) :
		Pipe(name, index) {
	}
	virtual void handle(const ObjectSeq& seq);
};	

};
};
};

#endif /* _NOTICEPASSAGE_H_ */
