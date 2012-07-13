#ifndef __PAGEFANSI_h__
#define __PAGEFANSI_h_

#include "ServiceI.h"
#include "Singleton.h"
#include "PageFans.h"
#include "PageFansObj.h"
#include "../data/PageFansCommon.h"
namespace xce{
namespace buddy{
using namespace ::com::xiaonei::xce;
using namespace xce::buddy;
using namespace MyUtil;

const static int PAGEFANS_DATA = 0;

const static int TASK_LEVEL_FILL=1;
const static int TASK_LEVEL_UPDATE=2;

class PageFansManagerI: virtual public PageFansManager,virtual public MyUtil::Singleton<PageFansManagerI> {
public:

        PageFansManagerI(){}
		virtual PageEntryPtr getFan(::Ice::Int pageId, ::Ice::Int userId,
			const Ice::Current& = Ice::Current());
        //virtual PageEntry getFanById(::Ice::Int id, ::Ice::Int pageId, ::Ice::Int userId,
		//	const Ice::Current& = Ice::Current());

		//	virtual MyUtil::IntSeq getFansIds(::Ice::Int pageId, ::Ice::Int begin , ::Ice::Int limit,
		//			const Ice::Current& = Ice::Current());
        virtual PageEntryList getFans( ::Ice::Int pageId,::Ice::Int begin , ::Ice::Int limit,
			const Ice::Current& = Ice::Current());

		//select * from page_fans where page_id={pageId} and user_id in {userIds}
        virtual PageEntryList getFansByUserIds(::Ice::Int pageId,const  MyUtil::IntSeq&  userIds,
			const Ice::Current& = Ice::Current());
		//select user_id from page_fans where page_id={pageId} and state={真正的粉丝} order by id desc limit {count}
        virtual PageEntryList getFansByState(::Ice::Int pageId, ::Ice::Int state, ::Ice::Int begin,  ::Ice::Int limit,
			const Ice::Current& = Ice::Current());

		//
        virtual ::Ice::Int getFansCountByState(::Ice::Int pageId, ::Ice::Int state,
			const Ice::Current& = Ice::Current());

		//select * from page_fans where page_id={pageId} and user_id={userid} and state={真正的粉丝}
        virtual bool isFan(::Ice::Int pageId, ::Ice::Int userId,
			const Ice::Current& = Ice::Current());


        //-- for fans ------
        virtual ::Ice::Int addFan(const PageEntryPtr& pe, const Ice::Current& = Ice::Current());
        virtual ::Ice::Int removeFan(::Ice::Int pageid, ::Ice::Int userid,
			const Ice::Current& = Ice::Current());

		//update page_fans set state={target.state} where id={target.id}
        virtual ::Ice::Int updateFanWithState(const PageEntryPtr& pe, const Ice::Current& = Ice::Current());

		//update page_fans set state={targetState} where page_id={pageId,} and state={oriState}
        virtual ::Ice::Int updateFansWithState(::Ice::Int pageId, ::Ice::Int oriState, ::Ice::Int tarState,
			const Ice::Current& = Ice::Current());

        //----for black list ---
        virtual ::Ice::Int addBlock(::Ice::Int pageId, ::Ice::Int userId, ::Ice::Int operatorId, ::Ice::Int timestamp,
			const Ice::Current& = Ice::Current());
        virtual ::Ice::Int removeBlock(::Ice::Int pageId, ::Ice::Int userId,
			const Ice::Current& = Ice::Current());
        virtual ::Ice::Int findBlock(::Ice::Int pageId, ::Ice::Int userId,
			const Ice::Current& = Ice::Current());
        virtual PageEntryList getBlockList(::Ice::Int pageId, const Ice::Current& = Ice::Current());

		virtual bool isValid(const Ice::Current& = ::Ice::Current());
		virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current()); 
		virtual void setData(const ::MyUtil::ObjectResultPtr& data, const ::Ice::Current& = ::Ice::Current());
private:

		bool _valid;
		IceUtil::RWRecMutex _validMutex;
		PageFansObjIPtr locatePageFans(::Ice::Int id) ;
	
};

class PageFansServantFactory : public MyUtil::ServantFactory, virtual public MyUtil::Singleton<PageFansServantFactory>
{
public:
        virtual Ice::ObjectPtr create(Ice::Int id);
};

class ServiceSchemaI: public MyUtil::Descriptor
{
public:
        virtual void configure(const Ice::PropertiesPtr& props);
};


class FillTask: virtual public ::MyUtil::Task 
{ 
		public:
				FillTask(const ::MyUtil::ObjectResultPtr& data):Task(TASK_LEVEL_FILL),_data(data)
		{}
				virtual void handle();

		private:
				::MyUtil::ObjectResultPtr _data;
};  //template FillTask


class UpdateTask: virtual public ::MyUtil::Task 
{ 
		public:
				UpdateTask(int pageId , int oriState , int tarState):Task(TASK_LEVEL_UPDATE),
				_pageId(pageId),_oriState(oriState),_tarState(tarState)
	 	{}
				virtual void handle();

		private:
				int _pageId;
				int _oriState;
				int _tarState;
};  //template FillTask


class PageFansResultHandlerForCache : public com::xiaonei::xce::ResultHandler {
		public:
				PageFansResultHandlerForCache(const MyUtil::ObjectResultPtr& results) : _results(results){};
				virtual bool handle(mysqlpp::Row& row) const {

						int userId = (int)row["user_id"];
						int id = (int)row["id"];

						int state = (int)row["state"];
						int addTime = (int)row["add_time"];
						int oper = (int)row["operator"];
						int pageId = (int)row["page_id"];


						PageEntryPtr ptr = new PageEntry(id, pageId, userId, state, addTime, oper);

						MyUtil::ObjectMap::iterator it = _results->data.find(pageId);
						if (it == _results->data.end())
						{}  
						else
						{   
								if(state == NORMAL)
								{
										PageFansObjIPtr::dynamicCast(it->second)->realfans.push_back(ptr); 
								}
								else
								{
										PageFansObjIPtr::dynamicCast(it->second)->otherfans.push_back(ptr); 
								}

						}   

						return true;
				}

		private:

				MyUtil::ObjectResultPtr _results;
};

class PageEntryBlockResultHandlerForCache : public com::xiaonei::xce::ResultHandler {
		public:
				PageEntryBlockResultHandlerForCache(const MyUtil::ObjectResultPtr& results) : _results(results){};
				virtual bool handle(mysqlpp::Row& row) const {

						int userId = (int)row["userid"];
						int id = (int)row["id"];
						int state = BLOCK;
						int addTime = (int)row["timestamp"];
						int operatorId = (int)row["executor"];
						int pageId = (int)row["pageid"];


						PageEntryPtr ptr = new PageEntry(id, pageId, userId, state, addTime, operatorId);

						MyUtil::ObjectMap::iterator it = _results->data.find(pageId);
						if (it == _results->data.end())
						{}
						else{
								PageFansObjIPtr::dynamicCast(it->second)->blockfans.push_back(ptr);
						}

						return true;
				}

		private:

				MyUtil::ObjectResultPtr _results;
};




}
}

#endif
