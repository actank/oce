#ifndef __COMMON_H__
#define __COMMON_H__

#include "ServiceI.h"
#include <bitset>

namespace xce {
namespace generalcheck {

const int ID_BATCH_SIZE = 3000;
const int STATUS_CACHE_SIZE = 2147483647;

class CheckIdData : public Ice::Object {
public:
	CheckIdData( MyUtil::LongSeq ids, const int mod ) : m_ids( ids ), m_mod( mod ) {
	}
	MyUtil::LongSeq m_ids;
	int m_mod;
};
typedef IceUtil::Handle<CheckIdData> CheckIdDataPtr;
typedef std::vector<CheckIdDataPtr> CheckIdDataSeq;

class CheckData : public Ice::Object {
public:
	CheckData(int count_) {
		count = count_;
	}
	int count;
};

typedef IceUtil::Handle<CheckData> CheckDataPtr;

class StatusData : public Ice::Object {
public:
	StatusData() {
	}
	StatusData(const std::bitset<STATUS_CACHE_SIZE>& status) {
		status_ = status;
	}
	std::bitset<STATUS_CACHE_SIZE> status_;	
};

typedef IceUtil::Handle<StatusData> StatusDataPtr;
}
}
#endif
