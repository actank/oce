#ifndef UNIVERSITYCACHERELOADERADAPTER_H_
#define UNIVERSITYCACHERELOADERADAPTER_H_

#include <UniversityCache.h>
#include <AdapterI.h>

namespace xce {
namespace universitycache {

class UniversityCacheReloaderAdapter : public MyUtil::ReplicatedClusterAdapterI<UniversityCacheReloaderPrx>, public MyUtil::Singleton<UniversityCacheReloaderAdapter>{
public:
	UniversityCacheReloaderAdapter();
	void AddUniversity(const UniversityCacheUnitSeq& data);
	void RemoveUniversity(const UniversityCacheUnitSeq& data);
	void Reload(const MyUtil::IntSeq& ids);
};

}
}

#endif
