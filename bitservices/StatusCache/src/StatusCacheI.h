#ifndef __STATUS_CACHEI_H__
#define __STATUS_CACHEI_H__

#include <StatusCache.h>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <bitset>
#include <Singleton.h>
#include <AtomicSTL.h>

namespace xce {
namespace statuscache {

const int STATUS_CACHE_SIZE = 2147483647;

class StatusCacheResultHandler : virtual public com::xiaonei::xce::ResultHandler
{
	public:
		StatusCacheResultHandler(int& large_id, map<int, int>& result) :
			_large_id(large_id), _result(result) {
		}
	protected:
		virtual bool handle(mysqlpp::Row& row) const;
	private:
		int& _large_id;
		map<int,int>& _result;
};

class StatusCacheManagerI: virtual public StatusCacheManager,
        virtual public MyUtil::Singleton<StatusCacheManagerI>/*, virtual public IceUtil::Thread */{
public:
	StatusCacheManagerI();
        void initialize();
	virtual MyUtil::ByteSeq getStatusMinMax(::Ice::Int,::Ice::Int,
			const Ice::Current&);
	virtual Int2BoolMap getStatusBatch(const MyUtil::IntSeq&,
			const Ice::Current&);
	virtual MyUtil::IntSeq getStatusIdsFrom(const MyUtil::IntSeq&,
			const Ice::Current&);
	virtual bool isStatusTrue(::Ice::Int,
			const Ice::Current&);
	virtual void updateStatus(::Ice::Int,::Ice::Int,
			const Ice::Current&);
        virtual void updateStatusBatch(const ::MyUtil::Int2IntMap&,
			const Ice::Current&);
	virtual bool isGoodUser(::Ice::Int,
			const Ice::Current&);
	virtual bool isValid(const Ice::Current& = Ice::Current());
private:
//	virtual void run();
	int getMaxId();
	void setMaxId(int id);

        bool _inited_flag;
	IceUtil::RWRecMutex _inited_flag_mutex;

	int maxId_;
	IceUtil::Monitor<IceUtil::Mutex> maxmutex_;

	std::bitset<STATUS_CACHE_SIZE> _status;
//	xce::atomicstl::BlockingQueue<std::pair<int, int> > queue_;
};

class FillTask : public MyUtil::Task {
public:
	virtual void handle();
};

}

}

#endif
