#ifndef __HOTWORD_ADAPTER_H__
#define __HOTWORD_ADAPTER_H__

#include "HotWord.h"
#include "ContentSearch.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"

namespace xce{
namespace hotwords{

class HotWordAdapter : public MyUtil::AdapterI,
	public MyUtil::AdapterISingleton</*MyUtil::SearchFoFChannel*/MyUtil::BlogSearchChannel, HotWordAdapter> {
public:
	HotWordAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
	}
	void updateDoings(const com::xiaonei::search5::logicserver::IndexUpdateDataSeq& infos);
private:
	virtual string name() {
		return "M";
	}
	virtual string endpoints() {
		return "@HotWord";
	}
	virtual size_t cluster() {
		return 1;
	}
	xce::hotwords::HotWordManagerPrx getHotWordManager(int id);
  //vector<int> getHotWord(int hostId, string condition, int limit);
	vector<xce::hotwords::HotWordManagerPrx> _managers;
};

}
;
}
;

#endif

