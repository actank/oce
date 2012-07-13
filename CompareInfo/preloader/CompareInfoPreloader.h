#ifndef __COMPAREINFOPRELOADER_H__
#define __COMPAREINFOPRELOADER_H__

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include <CompareInfo.h>

namespace xce
{

namespace compareinfo
{

const int PRODUCE_BATCH_SIZE = 1000;
const int CONSUME_BATCH_SIZE = 2000;
const int CONSUME_BATCH_THREAD_SIZE = 5;
const int WRITE_BATCH_SIZE = 1000;
const int PRX_DATAQUEUE_MAX_SIZE = 100;



class CompareInfoPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<CompareInfoPreloader> 
{
public:
        CompareInfoPreloader () {}
        virtual ~CompareInfoPreloader () {}
};

class CompareInfoPreloaderFactory : virtual public xce::generalpreloader::IdListFactory
{
public:
        CompareInfoPreloaderFactory() {}
        virtual ~CompareInfoPreloaderFactory() {}
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
	virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId);
};

}
}

#endif

