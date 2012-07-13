/*
 * =====================================================================================
 *
 *       Filename:  FootprintViewI.h
 *
 *    Description:  A view of Footprint for CS.
 *
 *        Version:  1.0
 *        Created:  2011年07月04日 15时25分10秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __FOOTPRINT_VIEW_I_H__
#define __FOOTPRINT_VIEW_I_H__


#include "../../share/FootprintShare.h"
#include "FootprintView.h"
#include "QueryRunner.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "Channel.h"
#include "Date.h"

#include <Ice/Ice.h>

namespace xce {
namespace footprint {

const int VISIT_COUNT_HISTORY = 0;
const int VISIT_COUNT_NUM = 31;
const uint VISITCOUNT_HISTORY_BATCH_SIZE = 1000;
const int ONE_DAY_SECOND = 24*60*60;

const std::string TABLE_VISIT_COUNT_HISTORY = "visitcount_history";
const int TABLE_VISIT_COUNT_HISTORY_SIZE = 100;
const std::string VISIT_COLUMN_OWNER = "owner";
const std::string VISIT_COLUMN_HISTORY = "history";
const std::string DESC_VISITCOUNT_HISTORY = "footprint";

class VisitCountHistorySet;
typedef IceUtil::Handle<VisitCountHistorySet> VisitCountHistorySetPtr;

class SetVisitCountHistoryThread;
typedef IceUtil::Handle<SetVisitCountHistoryThread> SetVisitCountHistoryThreadPtr;

class FootprintViewManagerI : virtual public FootprintViewManager, public MyUtil::Singleton<FootprintViewManagerI> {
public:
	FootprintViewManagerI();
	~FootprintViewManagerI();

	virtual bool verifyVisitCount(const MyUtil::Int2IntMap& counts, const Ice::Current& current);
	virtual VisitCountInfoSeq getVisitCountHistory(Ice::Int id, const Ice::Current& current);
	virtual VisitCountInfoPtr getRecentVisitCount(Ice::Int id, const Ice::Current& current);
	virtual void setVisitCountLimit(Ice::Int id, const Ice::Current& current);
	virtual void setVisitCountState(Ice::Int id, Ice::Int state, const Ice::Current& current);

	std::string getVisitCountHistoryTable(long id);

	VisitCountHistorySetPtr locateVisitCountHistory(long id);
private:
	SetVisitCountHistoryThreadPtr visit_count_threads[TABLE_VISIT_COUNT_HISTORY_SIZE];
};//end FootprintViewManagerI

class VisitCountHistorySet : public FootprintSet<VisitCountInfoPtr, FootprintViewManagerI> {
public:
	VisitCountHistorySet() : FootprintSet<VisitCountInfoPtr, FootprintViewManagerI>(VISIT_COUNT_NUM) {
	}

	void unserialize(const mysqlpp::Row::value_type& str) {
		vector<int> tmp;
		tmp.resize(str.size()/sizeof(int)); 
		memcpy((void*)(&tmp[0]), str.data(), tmp.size()*sizeof(int));

		ostringstream oss;
		IceUtil::Mutex::Lock lock(*this);
		
		for (size_t i = 0; i < tmp.size()/3; i++) {
			VisitCountInfoPtr vci = new VisitCountInfo;
			vci->timestamp = tmp[3*i];
			vci->count = tmp[3*i+1];
			vci->state = tmp[3*i+2];

			if (vci->timestamp > 0 && vci->count > 0) {
				footprints_.push_back(vci);
			}
		}
	}

	std::string serialize() {
		std::string result;
		std::vector<int> byteSeq;
		for (size_t i = 0; i < footprints_.size(); i++) {
			byteSeq.push_back(footprints_[i]->timestamp);
			byteSeq.push_back(footprints_[i]->count);
			byteSeq.push_back(footprints_[i]->state);
		}
		result.resize(footprints_.size() * 3 * sizeof(int));
		memcpy((void*)(result.data()), &byteSeq[0], footprints_.size()*3*sizeof(int));
		return result;
	}

	virtual bool add(const VisitCountInfoPtr& info) {

		VisitCountInfoPtr top = getTop();
		if (!top) {
			IceUtil::Mutex::Lock lock(*this);
			footprints_.push_front(info);
			return true;
		}

	//	if (top->state == STATELIMIT) {
	//		return true;
	//	}

		if (top->state == STATESPAMMER) {
			top->count = info->count;
	//		return true;
		}

		if (top->count > info->count) {
			return false;
		}
		
		if (getDate(top->timestamp) == getDate(info->timestamp)) {
			top->timestamp = info->timestamp;
			top->count = info->count;
		} else {
			IceUtil::Mutex::Lock lock(*this);
			footprints_.push_front(info);
		}
		return true;
	}

	std::vector<VisitCountInfoPtr> getAll(){
		IceUtil::Mutex::Lock lock(*this);
		std::vector<VisitCountInfoPtr> result;
		result.reserve(footprints_.size());
		for (boost::circular_buffer<VisitCountInfoPtr>::const_iterator it = footprints_.begin(); it != footprints_.end(); ++it) {
			result.push_back(*it);
		}
		return result;
	}
private:
	int getDate(int time) {
		char date[10];
		time_t data_tmp = time;
		strftime(date, sizeof(date), "%d", localtime(&data_tmp));
		return atoi(date);
	}
};//end VisitCountHistorySet

class SetVisitCountHistoryThread : public IceUtil::Thread {
public:
	SetVisitCountHistoryThread(int id) : id_(id) {};
	void push(long owner) {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
		owners_.insert(owner);
		if (owners_.size() > VISITCOUNT_HISTORY_BATCH_SIZE) {
			mutex_.notify();
		}
	}

	virtual void run();
private:
	IceUtil::Monitor<IceUtil::Mutex> mutex_;
	std::set<long> owners_;
	int id_;
};

class VisitCountHistoryHandler : public com::xiaonei::xce::ResultHandler {
public:
	VisitCountHistoryHandler(VisitCountHistorySetPtr& vch) : vch_(vch){};
	virtual bool handle(mysqlpp::Row& row) const;
private:
	VisitCountHistorySetPtr vch_;
};//end VisitCountHistroyHandler

class VisitCountHistoryFactoryI : public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(long id);
};//end VisitCountHistoryFactoryI


};
};

#endif
