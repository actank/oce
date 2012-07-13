#ifndef __STEP_H__
#define __STEP_H__

#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include <cmath>
#include "IceLogger.h"
namespace mop
{
namespace hi
{
namespace svc
{
namespace gf
{
	using namespace std;
	class Step:public Ice::Object
	{
		public:
		virtual void add(int time){}
		virtual int getAvgCount(int fromid, int toid){}
		virtual void incMark(){}
		virtual void decMark(){}
		virtual int getMark(){}
		virtual ~Step(){}
	};
	typedef IceUtil::Handle<Step> StepPtr;
};
};
};
};
#endif
