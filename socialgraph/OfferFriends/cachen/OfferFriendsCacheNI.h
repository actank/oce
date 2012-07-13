#ifndef __OFFERFRIENDS_CACHE_I_H__
#define __OFFERFRIENDS_CACHE_I_H__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include <OfferFriends.h>
#include <QueryRunner.h>
#include "ObjectCacheI.h"
#include "../share/OfferCacheDataI.h"
#include "../share/OfferCacheDataFactory.h"
#include "../share/OfferFriendsUtil.h"
#include "../share/OfferCreator.h"

namespace xce {
namespace offerfriends {

int rebuildInterval;

const int OFFER_CACHE_DATA_I = 0;
const int HIGHSCHOOL_CACHE_DATA_I = 4;

const int TASK_LEVEL_FILL = 1;
const int TASK_LEVEL_LOAD = 2;
const int TASK_LEVEL_REPORT = 3;

//**************************************************************************************************************

class OfferFriendsCacheNManagerI: virtual public OfferFriendsCacheNManager, 
		public MyUtil::Singleton<OfferFriendsCacheNManagerI>{
public:
	/*@brief 初始化函数  ??用它做什么了?
  * @param endpoint 初始化_endpoint
  * @param mod 初始化_mod
  */
	void bootstrap(const string& endpoint, 
								 int mod);

	virtual ::xce::offerfriends::OfferDataNSeq getFastOffer(::Ice::Int,
																												  ::Ice::Int,
																													const Ice::Current&);

	virtual ::xce::offerfriends::OfferDataNSeq getBestOffer(::Ice::Int,
																													::Ice::Int,
																													const Ice::Current&);

	virtual ::xce::offerfriends::OfferDataNSeq getRandomOffer(::Ice::Int,
																														::Ice::Int,
																														const Ice::Current&);

	
	/*@brief 根据不同阶段用户进行推荐 
	* 阶段分为HIGHSCHOOL WORK UNIV三个，中学用户，白领，大学生用户
	*/
	virtual ::xce::offerfriends::OfferDataNSeq getBestOfferByStage(::Ice::Int,
																																 ::Ice::Int,
																																 ::Ice::Int,
																																 const Ice::Current&);

	virtual ::xce::offerfriends::RmInfo apply(::Ice::Int,
																						::Ice::Int,
																						const Ice::Current&);

	virtual ::xce::offerfriends::RmInfo accept(::Ice::Int,
																						 ::Ice::Int,
																						 const Ice::Current&);

	virtual ::xce::offerfriends::RmInfo denyOffer(::Ice::Int,
																								::Ice::Int,
																								const Ice::Current&);

	virtual ::xce::offerfriends::RmInfo ignoreOffer(::Ice::Int,
																									::Ice::Int,
																									const Ice::Current&);

	virtual void load(Ice::Int userId,
										const Ice::ObjectPtr& data, 
										const Ice::Current&);

	virtual bool isValid(const Ice::Current&);

	virtual void setValid(bool valid, 
											  const Ice::Current&);

	virtual void setData(const MyUtil::ObjectResultPtr& datas, 
											 const Ice::Current&);

private:
	/*@brief 获取中学用户的推荐
  * @param userId 用户id 
  * @param limit 获取数量
  */
	::xce::offerfriends::OfferDataNSeq getHighSchoolRecommend(int userId, int limit);

	string _endpoint;
	int _mod;

	bool _isValid;
	IceUtil::RWRecMutex _validMutex;

	friend class MyUtil::Singleton<OfferFriendsCacheNManagerI>;
	OfferFriendsCacheNManagerI(){
	}

	OfferCacheDataIPtr locateHighSchoolCacheDataI(::Ice::Int id);
	OfferCacheDataIPtr locateOfferCacheDataI(::Ice::Int id);
	OfferCacheDataIPtr findOfferCacheDataI(::Ice::Int id);
};

//**************************************************************************************************************

/*@brief 创建ObjectCache的Factory类，用来产生推荐的OfferFriends对象*/
class OfferFriendsCacheNFactory : virtual public MyUtil::ServantFactory, 
		virtual public MyUtil::Singleton<OfferFriendsCacheNFactory> {
protected:
	friend class MyUtil::Singleton<OfferFriendsCacheNFactory>;

public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};

//*****************************************************************************

/*@brief 创建ObjectCache的Factory类，用来产生推荐的中学推荐对象*/
class HighSchoolCacheNFactory: virtual public MyUtil::ServantFactory,
		virtual public MyUtil::Singleton<HighSchoolCacheNFactory> {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);

protected:
	friend class MyUtil::Singleton<HighSchoolCacheNFactory>;
};

//*****************************************************************************

class FillTask: virtual public MyUtil::Task {
public:
	FillTask(const MyUtil::ObjectResultPtr& objs,::Ice::Int level=TASK_LEVEL_FILL):
		MyUtil::Task(level), _objs(objs) { }
	virtual void handle();

private:
	MyUtil::ObjectResultPtr _objs;
};

//***************************************************************************

class LoadTask: virtual public MyUtil::Task {
public:
	LoadTask(Ice::Int id, const Ice::ObjectPtr& obj, Ice::Int level=TASK_LEVEL_LOAD):
		MyUtil::Task(level), _id(id), _obj(obj)  { }
	virtual void handle();

private:
	Ice::Int _id;
	Ice::ObjectPtr _obj;
};

//***************************************************************************

class ReportTask: public MyUtil::Task{
public:
	ReportTask(::xce::offerfriends::GetInfo info, int level=TASK_LEVEL_REPORT):
		Task(level), _info(info){ };
	virtual void handle();

private:
        ::xce::offerfriends::GetInfo _info;
};

//***************************************************************************

class ServiceSchemaI: public MyUtil::Descriptor {
	public:
		virtual void configure(const Ice::PropertiesPtr& props);
};


//**************************************************************************************************************
}
}

#endif
