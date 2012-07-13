#ifndef __SCORE_LOADER_I_H__
#define __SCORE_LOADER_I_H__

#include <IceUtil/IceUtil.h>
#include <ServiceI.h>
#include "ReplicaCluster/CacheLoader/src/CacheLoader.h"

namespace xce {
namespace scorecache {

class ScorePreloaderI : virtual public MyUtil::Singleton<ScorePreloaderI> {
public:
	ScorePreloaderI(){}
	~ScorePreloaderI(){}
};

}
}

#endif
