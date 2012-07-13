#ifndef	_FRIENDFINDER_INFOWORKERI_H_ 
#define _FRIENDFINDER_INFOWORKERI_H_ 

#include "ServiceI.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "FriendFinderInfoWorker.h"
#include "InfoResultCollector.h"

namespace xce {
namespace friendfinder{

const static int VIEW_COUNT = 1;
const static int VIEW_INDEX = 1;

class ViewCountData : virtual public Ice::Object, virtual public IceUtil::Mutex {
public:
	void SetData(const std::map<int, int>& viewCountMap);
	void GetDataOrderedByCount(std::vector<int>& idSeq); 

private:
	std::map<int, int> _viewCountMap;
	IceUtil::RWRecMutex _init_flag_mutex;
};

typedef IceUtil::Handle<ViewCountData> ViewCountDataPtr;

//*************************************************************************************************

class FriendFinderInfoWorkerI: virtual public FriendFinderInfoWorker,
		virtual public MyUtil::Singleton<FriendFinderInfoWorkerI> {
public: 
	int g_workplace_task;
	int g_univ_task;
	int g_highschool_task;
	int g_juniorschool_task;
	int g_elementaryschool_task;
	int g_birthday_task;

	virtual void createDataFromDB(::Ice::Int, const Ice::Current&);
	virtual void restartControl(const Ice::Current&);

protected:
	friend class MyUtil::Singleton<FriendFinderInfoWorkerI>;

private:
	void CreateViewCountData();
};

//*************************************************************************************************

class ViewCountHandler: public com::xiaonei::xce::ResultHandler {
public:
	ViewCountHandler(int& last, std::map<int, int>& viewCountMap): 
		_last(last), _viewCountMap(viewCountMap) {}
	virtual bool handle(mysqlpp::Row& row) const;
						

private:
	int& _last;
	std::map<int, int>& _viewCountMap;
};

//*************************************************************************************************

class ViewCountFactory: virtual public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(::Ice::Int id);
};

}
}

#endif
