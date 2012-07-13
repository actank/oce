#ifndef __NEWCOMER_CACHE_DATA_I_H__
#define __NEWCOMER_CACHE_DATA_I_H__

#include "FriendFinderNewComer.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <ServiceI.h>
#include <iostream>

namespace xce {
namespace socialgraph {
namespace friendfindernewcomer {

	const int DATA_LIMIT = 30;
	const int DAYS_LIMIT = 30;
	
	class CompNewComerMcData : public std::greater<NewComerMcData>  {
	public:
		bool operator() (const NewComerMcData&, const NewComerMcData&);
	};

	class CompNewComerDbData : public std::greater<NewComerDbData> {
	public:
		bool operator() (const NewComerDbData&, const NewComerDbData&);
	};

	class NewComerCacheDataI: virtual public NewComerCacheData {
	public:
		NewComerCacheDataI();
		NewComerCacheDataI(const NewComerCacheDataPtr& obj);
		NewComerCacheDataI(const NewComerDbDataMap& db_objs);
		NewComerCacheDataI(vector<unsigned char>& mc_objs);
	public:
		vector<unsigned char> getBinData(int,MyUtil::Int2IntSeqMap&);
		vector<unsigned char> add(int,int,NewComerDbData&,MyUtil::Int2IntSeqMap& );
		vector<unsigned char> remove(RmInfo&,int);
	public:
		MyUtil::IntSeq getIds();
		NewComerDbDataMap getAll();
		NewComerDbDataMap getBest(const std::map<int,bool>&,int inDays,int limit = -1);
		NewComerDbDataMap getRandom(int inDays,int limit = -1);
		NewComerDbDataSeq getByType(int type,int inDays,int limit = -1);
	private:
		NewComerDbDataSeq doRandom(const NewComerDbDataSeq&,int);
		NewComerDbDataSeq doOnlineFilter(int&,const NewComerDbDataSeq&,int,const std::map<int,bool>&);
	private:
		IceUtil::Mutex _mutex;
	};
	typedef IceUtil::Handle<NewComerCacheDataI> NewComerCacheDataIPtr;

}
}
}

#endif
