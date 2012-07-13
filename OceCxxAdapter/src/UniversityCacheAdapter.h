#ifndef UNIVERSITYCACHEADAPTER_H_
#define UNIVERSITYCACHEADAPTER_H_

#include <UniversityCache.h>
#include <AdapterI.h>

namespace xce {
namespace universitycache {

class UniversityCacheAdapter : public MyUtil::ReplicatedClusterAdapterI<UniversityCacheManagerPrx>, public MyUtil::Singleton<UniversityCacheAdapter>{
public:
	UniversityCacheAdapter();
	void AddUniversity(const UniversityCacheUnitSeq& data);
	void RemoveUniversity(const UniversityCacheUnitSeq& data);
	void setData(const MyUtil::ObjectResultPtr& data);
	bool IsSameUniversity(int hostid, int guestid);
};

}
}

#endif
