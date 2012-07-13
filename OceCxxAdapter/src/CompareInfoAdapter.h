#ifndef COMPAREINFOADAPTER_H_
#define COMPAREINFOADAPTER_H_

#include <CompareInfo.h>
#include <AdapterI.h>

namespace xce {
namespace compareinfo {

class CompareInfoAdapter : public MyUtil::ReplicatedClusterAdapterI<CompareInfoManagerPrx>, public MyUtil::Singleton<CompareInfoAdapter>{
public:
	CompareInfoAdapter();
};

}
}

#endif

