#ifndef __FOOTPRINTFILTER_I_H__
#define __FOOTPRINTFILTER_I_H__

#include "FootprintUnite.h"
#include "Date.h"
#include "Step.h"
#include "UtilCxx/src/Singleton.h"
#include "IceExt/src/ServiceI.h"
#include "IceExt/src/Channel.h"
#include "DbCxxPool/src/QueryRunner.h"

#include <Ice/Ice.h>
#include <Ice/Exception.h>
#include <Freeze/Freeze.h>
#include <IceUtil/AbstractMutex.h>
#include <IceUtil/Mutex.h>
#include <boost/lexical_cast.hpp>
#include "XceLoggerAdapter.h"

using namespace MyUtil;
using namespace com::xiaonei::service;


namespace xce
{
namespace footprint 
{
const Ice::Int STEP = 0;
const Ice::Int VISITORMAP= 1;
const int TASK_LEVEL_BATCH = 1;

const int BLACK_VISITOR = 0;
const int WHITE_VISITOR = 1;
const int BLACK_OWNER = 2; 
const int WHITE_OWNER = 3; 

//---------------------------------------------------------------------------
class FootprintFilterFactoryI : public ServantFactory
{
public:
    virtual Ice::ObjectPtr create(Ice::Int id);
};

class VisitorMapFactoryI: public ServantFactory
{
public:
    virtual Ice::ObjectPtr create(Ice::Int id);
};

class VisitorMap:public Ice::Object{
public:
        map<int,int> id2Time_;
};
typedef IceUtil::Handle<VisitorMap> VisitorMapPtr;

//---------------------------------------------------------------------------
class FootprintFilterManagerI : virtual public mop::hi::svc::fpt::FootprintFilterManager,
	virtual public IceUtil::AbstractMutexI<IceUtil::Mutex>,
	virtual public MyUtil::Singleton <FootprintFilterManagerI>
{
public:
	FootprintFilterManagerI(int tableCount,const XceLoggerPtr& xce_logger, string loggerTopCategory,string privacy);

	virtual HomeInfoPtr visitAndGet(const FootprintInfoPtr& fpt, const Ice::Current& = Ice::Current());

	virtual bool visit(const FootprintInfoPtr& fpt, const Ice::Current& = Ice::Current());

	virtual void visitBatch(const FootprintInfoSeq& fpts, const Ice::Current& = Ice::Current());

	virtual void contral(const MyUtil::Str2IntMap& cmd, const Ice::Current& = Ice::Current());
	
	void dispatch(const FootprintInfoPtr& fpt);
	void dispatchProfile(const FootprintInfoPtr& fpt);
	void dispatchAlbum(const FootprintInfoPtr& fpt);
	void dispatchBlog(const FootprintInfoPtr& fpt);
	bool canPass(const FootprintInfoPtr& fpt);

private:
	/* @brief	通过step计算这个脚印是否可踩
	 * @param	fpt	当前被踩的脚印
	 * @retuan	该脚印是否可踩(即访问Footprint服务的踩脚印接口)
	 */
	bool stepFootprint(const FootprintInfoPtr& fpt, const map<string, string>& ctx);

	StepPtr getStep(Ice::Int userId);
	VisitorMapPtr getVisitorMap(Ice::Int userId);

	bool canSend(const FootprintInfoPtr & fpt);
	bool canRecv(const FootprintInfoPtr & fpt);
	bool canStranger(const FootprintInfoPtr & fpt);

	std::string _privacy;
	int _tableCount;
	XceLoggerPtr _xce_logger;
	MyUtil::StrSeq _loggerCategories;

	IceUtil::Mutex mutex_;
	vector<set<int> > filter_list_;
};

class BatchVisitTask : public MyUtil::Task {
public:
	BatchVisitTask(const FootprintInfoSeq& fpts, FootprintFilterManagerI& filter, const Ice::Current& crt):
				Task(TASK_LEVEL_BATCH),
				_fpts(fpts),
				_filter(filter),
				_crt(crt) {
	}
	virtual void handle();
private:
	FootprintInfoSeq _fpts;
	FootprintFilterManagerI& _filter;
	const Ice::Current& _crt;
};

class FootprintSendConfigHandler : public com::xiaonei::xce::ResultHandler
{
public:
	FootprintSendConfigHandler(bool &flag);
	virtual ~FootprintSendConfigHandler();
	virtual bool handle( mysqlpp::Row& row ) const;
private:
	bool &_flag;
};


class FootprintRecvConfigHandler : public com::xiaonei::xce::ResultHandler
{
public:
	FootprintRecvConfigHandler(bool &flag);
	virtual ~FootprintRecvConfigHandler();
	virtual bool handle( mysqlpp::Row& row ) const;
private:
	bool &_flag;
};

class FootprintStrangerConfigHandler : public com::xiaonei::xce::ResultHandler
{
public:
	FootprintStrangerConfigHandler(bool &flag);
	virtual ~FootprintStrangerConfigHandler();
	virtual bool handle(mysqlpp::Row& row) const;
private:
	bool &_flag;
};

};
};
#endif

