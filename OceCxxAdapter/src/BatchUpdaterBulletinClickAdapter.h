#ifndef __BATCHUPDATER_BULLETINCLICK_ADAPTER_H__
#define __BATCHUPDATER_BULLETINCLICK_ADAPTER_H__

#include "BatchUpdater.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"

namespace com  {
namespace xiaonei  {
namespace adapter  {

using namespace MyUtil;
using namespace mop::hi::svc::model;

class BatchUpdaterBulletinClickAdapter : public AdapterI,
	public AdapterISingleton<OceChannel, BatchUpdaterBulletinClickAdapter> {

public:

	void click(int userId, int bulletinId);

private:
	virtual void initialize();

	BatchUpdaterManagerPrx _batchUpdaterManager;
	BatchUpdaterManagerPrx _batchUpdaterManagerOneWay;

	virtual string name()  {
		return "BatchUpdaterBulletinClick";
	};

	virtual string endpoints() {
		return "@BatchUpdaterBulletinClick";
	};

	virtual size_t cluster() {
		return 0;
	};

};

};
};
};

#endif
