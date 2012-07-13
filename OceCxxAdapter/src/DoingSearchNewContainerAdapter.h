#ifndef __DOINGSEARCH_NEW_CONTAINER_ADAPTER_H__
#define __DOINGSEARCH_NEW_CONTAINER_ADAPTER_H__

#include <DoingSearch.h>
#include <Channel.h>
#include <Singleton.h>
#include <AdapterI.h>

namespace xce {
namespace doingsearch {

class DoingSearchNewContainerAdapter : public MyUtil::AdapterI,
	public MyUtil::AdapterISingleton<MyUtil::SearchChannel, DoingSearchNewContainerAdapter>,
	public Ice::Object{
public:
	DoingSearchNewContainerAdapter();
	void addDoing(DoingItem item);
	DoingSearchContainerPrx getDoingSearchContainer();
	DoingSearchContainerPrx getDoingSearchContainerOneway();
private:
	virtual std::string name();
	virtual std::string endpoints();
	virtual size_t cluster();
};
};
};
#endif

