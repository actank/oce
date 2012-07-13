#ifndef COMPAREINFORELOADERADAPTER_H_
#define COMPAREINFORELOADERADAPTER_H_

#include <CompareInfo.h>
#include <AdapterI.h>

namespace xce {
namespace compareinfo {

class CompareInfoReloaderAdapter : public MyUtil::ReplicatedClusterAdapterI<CompareInfoReloaderPrx>, public MyUtil::Singleton<CompareInfoReloaderAdapter>{
public:
	CompareInfoReloaderAdapter();
        void update(const xce::compareinfo::CompareInfoUpdateOldDataPtr& data);
};

}
}

#endif

