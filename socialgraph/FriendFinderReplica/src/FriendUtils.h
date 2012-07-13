#ifndef FRIENDUTILS_H_
#define FRIENDUTILS_H_

#include <Ice/Ice.h>
#include "Util.h"
#include "QueryRunner.h"
#include "BuddyCore.h"

namespace com {
namespace xiaonei {
namespace service {

/* This class is deprecated */
class RelationBatchHelper {
public:
	static MyUtil::Int2IntSeqMap getFriendSeqBatch(const MyUtil::IntSeq& users);
	static MyUtil::Int2IntSeqMap getApplySeqBatch(const MyUtil::IntSeq& users);
	static MyUtil::IntSeq getFriendSeq(int id);
	static MyUtil::IntSeq getApplySeq(int id);
	static MyUtil::IntSeq getBlockSeq(int id);
	static MyUtil::IntSeq getBlockedSeq(int id);
};

class CommonFriendsBlockHelper{
public:
	static MyUtil::IntSeq getCommonFriendsBlock(int id);
};

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

class RelationBatchResultHandler : public com::xiaonei::xce::ResultHandler {
public:
			RelationBatchResultHandler(MyUtil::Int2IntSeqMap& results, const std::string& fromField,
					const std::string& toField);
	virtual ~RelationBatchResultHandler();
protected:
	virtual bool handle(mysqlpp::Row& row) const;
private:
	MyUtil::Int2IntSeqMap& _results;
	std::string _fromField;
	std::string _toField;
};

class IntSeqSerialize {
public:
	static std::string serialize(MyUtil::IntSeq seq);
	static MyUtil::IntSeq unserialize(const mysqlpp::Row::value_type & str);
};

class BuddyCoreSnapshotHelper {
private:
	// static BuddyCoreSnapshotHelper getForCommonFriend(int userId);
	// static BuddyCoreSnapshotHelper getForShareFriend(const MyUtil::IntSeq& userIds);

	MyUtil::Int2IntSeqMap getFriendSeqBatch(const MyUtil::IntSeq& users);
	MyUtil::Int2IntSeqMap getApplySeqBatch(const MyUtil::IntSeq& users);
	MyUtil::IntSeq getFriendSeq(int id);
	MyUtil::IntSeq getApplySeq(int id);
	MyUtil::IntSeq getBlockSeq(int id);
private:
	com::xiaonei::service::buddy::SnapshotEntrySeqMap _data;

	MyUtil::IntSeq getSeq(int id, com::xiaonei::service::buddy::BuddyDesc desc);
	MyUtil::Int2IntSeqMap getBatch(const MyUtil::IntSeq& users, com::xiaonei::service::buddy::BuddyDesc desc);

	BuddyCoreSnapshotHelper(const com::xiaonei::service::buddy::SnapshotEntrySeqMap& data) :
		_data(data) {
	}
};

class TableName{
public:
	// Helper function for generating table name and pattern
	static std::string common_friends_table(int id) {
		ostringstream oss;
		oss << "common_friends_" << (id % 100);
		return oss.str();
		//return "common_friends";
	}
	static std::string common_friends_patten(int id) {
		ostringstream oss;
		oss << "common_friends_" << (id % 100);
		return oss.str();
		//return "";
	}
	static std::string common_friends_block_table(int id) {
		return "common_friends_block";
	}
	static std::string common_friends_block_patten(int id) {
		return "common_friends_block";
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
}
#endif
