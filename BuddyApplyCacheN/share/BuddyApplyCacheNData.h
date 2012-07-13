#ifndef __BUDDYAPPLYCACHEN__DATA_H__
#define __BUDDYAPPLYCACHEN__DATA_H__

#include "BuddyApplyCache.h"
#include <QueryRunner.h>
#include "util/cpp/MD5Util.h"
#include <Ice/Ice.h>

namespace xce {
namespace buddyapplycache {
class BuddyApplyCacheNI: virtual public Ice::Object,
	virtual public IceUtil::Mutex {
		public:
			friend class BuddyApplyCacheResultHandlerI;
			friend class BuddyApplyCacheMasterResultHandlerI;
			BuddyApplyCacheNI( BuddyApplyCacheNPtr ptr);
			BuddyApplyCacheDataSeqN get(int begin, int limit);
			BuddyApplyCacheDataSeqN getData();
			void add(BuddyApplyCacheDataN data);
			void remove(int applicant);
			void clear();
			void sort();
			int getTotalCount();
			bool hasExtra();
			void setExtra(bool extra);
			int getId();
			time_t getBasicTime();
			void setBasicTime(time_t basic);
		private:
			BuddyApplyCacheDataSeqN _buddyApplyCaches;
			const ::Ice::Int _ownerId;
			bool _extra;
			time_t _basicTime;
	};
typedef IceUtil::Handle<BuddyApplyCacheNI> BuddyApplyCacheNIPtr;


//***************************************************************************
struct equal_apply : public std::binary_function<BuddyApplyCacheDataN, BuddyApplyCacheDataN, bool> {
	bool operator()(const BuddyApplyCacheDataN& x, const BuddyApplyCacheDataN& y) const {
		return (x.applicant == y.applicant);
	}
};
//**************************************************************************************


//***************************************************************************

BuddyApplyCacheNI::BuddyApplyCacheNI(BuddyApplyCacheNPtr ptr): _ownerId ( ptr-> ownerId) {
		_buddyApplyCaches = ptr->buddyApplyCaches;
		_basicTime = time(0);
		setExtra(false);
		if(getTotalCount() == 100 ) {
				setExtra(true);
		} else {
				setExtra(false);
		}
}

BuddyApplyCacheDataSeqN BuddyApplyCacheNI::getData() {
		return _buddyApplyCaches;
}

BuddyApplyCacheDataSeqN BuddyApplyCacheNI::get(int begin, int limit) {
		IceUtil::Mutex::Lock lock(*this);
		MCE_INFO("[get] userId=" << _ownerId << " _buddyApplyCaches.size()=" << _buddyApplyCaches.size());
		BuddyApplyCacheDataSeqN res;
		for (BuddyApplyCacheDataSeqN::const_iterator it =
						_buddyApplyCaches.begin(); it != _buddyApplyCaches.end(); ++it) {
				if (begin-- > 0) {
						continue;
				}
				if (limit > 0 && (int)res.size() >= limit) {
						break;
				}
				res.push_back(*it);
		}
		return res;
}

void BuddyApplyCacheNI::add(BuddyApplyCacheDataN data) {
		IceUtil::Mutex::Lock lock(*this);
		BuddyApplyCacheDataSeqN::iterator it = find_if(_buddyApplyCaches.begin(), _buddyApplyCaches.end(), 
						bind2nd(equal_apply(), data));
		if( it != _buddyApplyCaches.end()){
				_buddyApplyCaches.erase(it);
		}
		if( _buddyApplyCaches.size() >= 100){
				_buddyApplyCaches.pop_back();
				_extra = true;
		}
		_buddyApplyCaches.insert(_buddyApplyCaches.begin(),data);
}

void BuddyApplyCacheNI::remove(int applicant) {
		IceUtil::Mutex::Lock lock(*this);
		BuddyApplyCacheDataN buddyapply;
		buddyapply.applicant = applicant;
		BuddyApplyCacheDataSeqN::iterator it = find_if(_buddyApplyCaches.begin(), _buddyApplyCaches.end(),   
						bind2nd(equal_apply(), buddyapply));
		if(it != _buddyApplyCaches.end()){
				_buddyApplyCaches.erase(it);
		}
}

void BuddyApplyCacheNI::clear() {
		IceUtil::Mutex::Lock lock(*this);
		_buddyApplyCaches.clear();
}

//void BuddyApplyCache::sort() {
//	IceUtil::Mutex::Lock lock(*this);
//	std::sort(_buddyApplyCaches.begin(), _buddyApplyCaches.end(), greater);
//}

int BuddyApplyCacheNI::getTotalCount() {
		IceUtil::Mutex::Lock lock(*this);
		if(_extra) {
				return 100;
		}
		return _buddyApplyCaches.size();
}

void BuddyApplyCacheNI::setExtra(bool extra) {
		IceUtil::Mutex::Lock lock(*this);
		_extra = extra;
}

bool BuddyApplyCacheNI::hasExtra() {
		IceUtil::Mutex::Lock lock(*this);
		return _extra;
}
int BuddyApplyCacheNI::getId(){
		return _ownerId;
}

time_t BuddyApplyCacheNI::getBasicTime(){
		return _basicTime;
}

void BuddyApplyCacheNI::setBasicTime(time_t basic){
	_basicTime = basic;
}
//------------------------------------------------------------


class BuddyApplyCacheResultHandlerI: public com::xiaonei::xce::ResultHandler {
		public:
				BuddyApplyCacheResultHandlerI(const BuddyApplyCacheNPtr& result);
				virtual bool handle(mysqlpp::Row&) const;
		private:
				BuddyApplyCacheNPtr _result;
};

BuddyApplyCacheResultHandlerI::BuddyApplyCacheResultHandlerI(
				const BuddyApplyCacheNPtr& result) :
		_result(result) {
		}

bool BuddyApplyCacheResultHandlerI::handle(mysqlpp::Row& row) const {
		BuddyApplyCacheDataN data;
		data.applicant = (int) row["applicant"];
		data.time = (int) row["unix_time"];
		data.why = (string) row["why"];
		data.statfrom = (string) row["stat_from"];
		data.groupname = (string) row["group_name"];
		_result->buddyApplyCaches.push_back(data);
		return true;
}

class  BuddyApplyCacheMasterResultHandlerI : public com::xiaonei::xce::ResultHandler {
		public:
				BuddyApplyCacheMasterResultHandlerI(const BuddyApplyCacheNPtr& result);
				virtual bool handle(mysqlpp::Row&) const;
		private:
				BuddyApplyCacheNPtr _result;
};

BuddyApplyCacheMasterResultHandlerI::BuddyApplyCacheMasterResultHandlerI(
				const BuddyApplyCacheNPtr& result) :
		_result(result) {
		}


bool BuddyApplyCacheMasterResultHandlerI::handle(mysqlpp::Row& row) const {
		BuddyApplyCacheDataN data;
		data.applicant = (int) row["applicant"];
		data.time = (int) row["unix_time"];
		data.why = (string) row["why"];
		_result->buddyApplyCaches.push_back(data);
		return true;
}


}

}

#endif

