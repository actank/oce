#ifndef __FASTPRELOADERSHARE_H__
#define __FASTPRELOADERSHARE_H__

#include <stdint.h>
#include <IceUtil/IceUtil.h>
#include <ObjectCacheI.h>
#include "UtilCxx/src/AtomicSTL.h"
#include "UtilCxx/src/LogWrapper.h"

namespace xce{
namespace fastpreloader{

class BlockingQueueInt : 
			public virtual xce::atomicstl::BlockingQueue<int>, 
			public virtual Ice::Object {
};
typedef IceUtil::Handle<BlockingQueueInt> BlockingQueueIntPtr;

class FastPreloaderFactory : 
			public IceUtil::Shared {
public:
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids) = 0;

};
typedef IceUtil::Handle<FastPreloaderFactory> FastPreloaderFactoryPtr;


}
}

#endif
