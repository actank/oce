#ifndef __CHARITY_ADAPTER_H__
#define __CHARITY_ADAPTER_H__

#include "Charity.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace charity {

using namespace xce::charity;
using namespace MyUtil;

const int CLUSTER = 1;

class CharityAdapter : public MyUtil::AdapterI,
	public MyUtil::AdapterISingleton<MyUtil::WServiceChannel, CharityAdapter>
{
public:
	CharityAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);

	}
	void set(int id, bool flag);

private:
		
	virtual string name() {
		return "Charity";
	}
	;
	virtual string endpoints() {
		return "@Charity";
	}
	;
	virtual size_t cluster() {
		return CLUSTER;
	}
	;

	CharityManagerPrx getCharityManager(int userId);
	CharityManagerPrx getCharityManagerOneway(int userId);

	vector<CharityManagerPrx> _managers;
	vector<CharityManagerPrx> _managersOneway;

};

}
}

#endif

