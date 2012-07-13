#ifndef FRIENDUTILS_H_
#define FRIENDUTILS_H_

#include <Ice/Ice.h>
#include "Util.h"
#include "QueryRunner.h"
#include "BuddyCore.h"

namespace xce {
namespace socialgraph {

class RelationBatchHelper {
public:
	static void GetFriendSeqBatch(const MyUtil::IntSeq& users, MyUtil::Int2IntSeqMap& ff); 
	static void GetFriendSeq(int id, MyUtil::IntSeq& friendlist); 
	static void GetApplySeq(int id, MyUtil::IntSeq& applylist); 
	static void GetBlockSeq(int id, MyUtil::IntSeq& blocklist); 
	static void GetCommonFriendsBlock(int id, MyUtil::IntSeq& commonblock); 
};

//--------------------------------------------------------------------

class RelationResultHandler : public com::xiaonei::xce::ResultHandler {
public:
	RelationResultHandler(MyUtil::IntSeq& results, const std::string& toField);
	virtual ~RelationResultHandler();
protected:
	virtual bool handle(mysqlpp::Row& row) const;
private:
	MyUtil::IntSeq& _results;
	std::string _toField;
};

//--------------------------------------------------------------------

class TableName{
public:
	static std::string common_friends_table(int id) {
		ostringstream oss;
		oss << "common_friends_" << (id % 100);
		return oss.str();
	}
	static std::string common_friends_pattern(int id) {
		ostringstream oss;
		oss << "common_friends_" << (id % 100);
		return oss.str();
	}
	static std::string common_friends_block_table(int id) {
		ostringstream oss;
		oss << "common_friends_block_" << (id % 100);
		return oss.str();
	}
	static std::string common_friends_block_pattern(int id) {
		ostringstream oss;
		oss << "common_friends_block_" << (id % 100);
		return oss.str();
	}

	static std::string buddyapply_friendfinder_applicant(int id) {
		ostringstream oss;
		oss << "buddyapply_applicant_" << (id % 100);
		return oss.str();
	}

	static std::string buddyapply_friendfinder_applicant_pattern(int id) {
		ostringstream oss;
		oss << "buddyapply_applicant_" << (id % 100);
		return oss.str();
	}
};

}
}
#endif
