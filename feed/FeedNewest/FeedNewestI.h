#include<Ice/Ice.h>
#include<FeedNewest.h>
#include<Singleton.h>
#include<IceUtil/Handle.h>
#include<boost/multi_index_container.hpp>
#include<boost/multi_index/member.hpp>
#include<boost/multi_index/ordered_index.hpp>
#include<boost/multi_index/sequenced_index.hpp>
#include<boost/multi_index/hashed_index.hpp>
#include"TaskManager.h"
using namespace std;
using namespace demo;
using namespace MyUtil;

typedef IceUtil::Handle<NewestFeedIdMap> NewestFeedIdMapPtr;

class FeedNewestI :public FeedNewest,public Singleton<FeedNewestI>
{
	public:
		virtual void set(const int uid, const long feedid, 
                                 const int type,const Ice::Current&);
		virtual NewestFeedIdMap get(const int uid, 
                                            const Ice::Current&);
                bool getStat(){return dirty;}
		void setStat(bool stat){dirty = stat;}
	private:
		bool dirty;
		typedef boost::multi_index_container<
		NewestFeedIdMapPtr,
		    indexde_by<
		      sequenced<> ,
		      ordered_unique<
			    BOOST_MULTI_INDEX_MEMBER(NewestFeedIdMapPtr, int,NewestFeedIdMap.first())
		      >
		    >
		>NewestFeedIdMapContainer;
		typedef NewestFeedIdMapContainer::nth_index<0>::type SeqIndex;
		typedef NewestFeedIdMapContainer::nth_index<1>::type UidIndex;
		typedef NewestFeedIdMapContainer::iterator iterator;

		NewestFeedIdMapContainer container_;
		IceUtil::Mutex mutex_;

		
};
class FeedNewestTask : public Task
{
	public:
		virtual void handle();
};
class FeedNewestTimer : public Timer
{
	public:
		FeedNewestTimer() : Timer(60*10000){};
		virtual void handle();
};
