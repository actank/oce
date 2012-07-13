#include "FriendUtils.h"
#include "ServiceI.h"
#include "BuddyByIdReplicaAdapter.h"

using namespace ::com::xiaonei::service;
using namespace ::com::xiaonei::service::buddy;
using namespace ::xce::buddy::adapter;
using namespace ::MyUtil;
using namespace ::com::xiaonei::xce;

MyUtil::Int2IntSeqMap RelationBatchHelper::getFriendSeqBatch(const MyUtil::IntSeq& users) {
	Int2IntSeqMap result;
	if (users.empty()) {
		return result;
	}
	return BuddyByIdCacheAdapter::instance().getFriendLists(users);
	/*for(Int2IntSeqMap::iterator it=result.begin();it!=result.end();it++){
		MCE_DEBUG("RelationBatchHelper::getFriendSeqBatch get size: " << it->second.size());
	}*/
	/*Int2IntSeqMap seperatedIds;
	for (IntSeq::const_iterator it = users.begin(); it != users.end(); ++it) {
		seperatedIds[*it%100].push_back(*it);
	}

	for (Int2IntSeqMap::iterator it = seperatedIds.begin(); it != seperatedIds.end(); ++it) {
		IntSeq ids = it->second;
		if (ids.empty()) {
			continue;
		}

		ostringstream table;
		table << "relation_" << it->first;

		ostringstream oss;
		IntSeq::iterator subIt = ids.begin();
		oss << "SELECT host, guest FROM " << table.str() << " WHERE host IN (" << *subIt;
		result.insert(make_pair(*subIt, IntSeq()));
		for (; subIt != ids.end(); ++subIt) {
			result.insert(make_pair(*subIt, IntSeq()));
			oss << ", " << *subIt;
		}
		oss << ")";
		RelationBatchResultHandler rsh(result, "host", "guest");
		QueryRunner("friendfinder_friend", CDbRServer, table.str()).query(Statement(oss.str()), rsh);
	}
	return result;*/
}

MyUtil::Int2IntSeqMap RelationBatchHelper::getApplySeqBatch(const MyUtil::IntSeq& users) {   
	MCE_DEBUG("[RelationBatchHelper] getApplySeqBatch users size=" << users.size());
	Int2IntSeqMap result;
	//if (users.empty()) {
	//	return result;
	//}
	//Statement oss;
	//IntSeq::const_iterator subIt = users.begin();
	//oss << "SELECT applicant, accepter FROM buddyapply WHERE applicant IN (" << *subIt;
	//for (; subIt != users.end(); ++subIt) {
	//	oss << ", " << *subIt;
	//}
	//oss << ") LIMIT " << users.size() * 100;
	//RelationBatchResultHandler rsh(result, "applicant", "accepter");
	//QueryRunner("friendfinder_apply", CDbRServer).query(oss, rsh);   //该数据源直接联到主库并且这个批量接口并没有使用，暂时废除该接口 
	return result;
}

MyUtil::IntSeq RelationBatchHelper::getFriendSeq(int id) {
	MCE_DEBUG("RelationBatchHelper::getFriendSeq for " << id);
	/*IntSeq result;
	if (id<=0) {
		return result;
	}
	ostringstream table;
	table << "relation_" << (id % 100);
	ostringstream oss;
	MCE_DEBUG("RelationBatchHelper::getFriendSeq A" << id);
	oss << "SELECT guest FROM " << table.str() << " WHERE host = " << id;
	MCE_DEBUG("RelationBatchHelper::getFriendSeq B " << id);
	RelationResultHandler rsh(result, "guest");
	MCE_DEBUG("RelationBatchHelper::getFriendSeq C " << oss.str());
	QueryRunner("friendfinder_friend", CDbRServer, table.str() ).query(Statement(oss.str()), rsh);
	MCE_DEBUG("RelationBatchHelper::getFriendSeq done " << id);*/
	return BuddyByIdCacheAdapter::instance().getFriendListAsc(id,-1);
	//MCE_DEBUG("RelationBatchHelper::getFriendSeq get size: " << result.size());
	//return result;
	//return result;
}

MyUtil::IntSeq RelationBatchHelper::getApplySeq(int id) {
	IntSeq result;
	if (id<=0) {
		return result;
	}
	//Statement oss;
	//oss << "SELECT accepter FROM buddyapply WHERE applicant = " << id << " LIMIT 100";
	//RelationResultHandler rsh(result, "accepter");
	//QueryRunner("friendfinder_apply", CDbRServer).query(oss, rsh);

	string pattern = TableName::buddyapply_friendfinder_applicant_pattern(id);	

	Statement oss;
	oss << "SELECT accepter FROM " <<  TableName::buddyapply_friendfinder_applicant(id)
		<< " WHERE applicant = " << id << " LIMIT 100";

	RelationResultHandler rsh(result, "accepter");
	QueryRunner("buddyapply_friendfinder_applicant", CDbRServer, pattern).query(oss, rsh);

	MCE_DEBUG("[RelationResultHandler] getApplySeq id=" << id << " result size=" << result.size() <<
				" pattern(" << pattern << ")  tablename(" << TableName::buddyapply_friendfinder_applicant(id) << ")");

	return result;
}

MyUtil::IntSeq RelationBatchHelper::getBlockSeq(int id) {
	IntSeq result;
	if (id<=0) {
		return result;
	}
	Statement oss;
	oss << "SELECT guest FROM relation_block  WHERE host = " << id;
	RelationResultHandler rsh(result, "guest");
	QueryRunner("friendfinder_block", CDbRServer).query(oss, rsh);
	return result;
}

MyUtil::IntSeq RelationBatchHelper::getBlockedSeq(int id) {
	IntSeq result;
	if (id<=0) {
		return result;
	}
	Statement oss;
	oss << "SELECT host FROM relation_block  WHERE guest = " << id;
	RelationResultHandler rsh(result, "host");
	QueryRunner("friendfinder_block", CDbRServer).query(oss, rsh);
	return result;
}

MyUtil::IntSeq CommonFriendsBlockHelper::getCommonFriendsBlock(int id) {
	IntSeq result;
	if (id<=0) {
		return result;
	}
	Statement oss;
	oss << "SELECT block_id FROM "<<TableName::common_friends_block_table(id)<<" WHERE owner_id = " << id;
	RelationResultHandler rsh(result, "block_id");
	QueryRunner("friendfinder_common_friends", CDbRServer, TableName::common_friends_block_patten(id)).query(oss, rsh);
	return result;
}

RelationResultHandler::RelationResultHandler(MyUtil::IntSeq& results, const std::string& toField) :
	_results(results), _toField(toField) {
}

RelationResultHandler::~RelationResultHandler() {
}

bool RelationResultHandler::handle(mysqlpp::Row& row) const {
	int to = (int)row[_toField.c_str()];
	_results.push_back(to);
	return true;
}

RelationBatchResultHandler::RelationBatchResultHandler(MyUtil::Int2IntSeqMap& results, const std::string& fromField,
		const std::string& toField) :
	_results(results), _fromField(fromField), _toField(toField) {
}

RelationBatchResultHandler::~RelationBatchResultHandler() {
}

bool RelationBatchResultHandler::handle(mysqlpp::Row& row) const {
	int from = (int) row[_fromField.c_str()];
	int to = (int)row[_toField.c_str()];
	_results[from].push_back(to);
	return true;
}

std::string IntSeqSerialize::serialize(MyUtil::IntSeq seq) {
	string result;
	result.resize(seq.size() * sizeof(int));
	memcpy((void*)(result.data()), &seq[0], seq.size()*sizeof(int));
	return result;
}

MyUtil::IntSeq IntSeqSerialize::unserialize(const mysqlpp::Row::value_type & str) {
	MyUtil::IntSeq result;
	// result.resize(str.length()/sizeof(int));
	int strLen = str.length();
	int resultLen = (strLen <= 0)?(0):((strLen-1)/sizeof(int)+1);
	result.resize(resultLen);

	memcpy((void*)(&result[0]), str.data(), str.length());
	return result;
}

// **************************************************************************

MyUtil::Int2IntSeqMap BuddyCoreSnapshotHelper::getFriendSeqBatch(const MyUtil::IntSeq& users) {
	return getBatch(users, Friend);
}
MyUtil::Int2IntSeqMap BuddyCoreSnapshotHelper::getApplySeqBatch(const MyUtil::IntSeq& users) {
	return getBatch(users, Apply);
}
MyUtil::IntSeq BuddyCoreSnapshotHelper::getFriendSeq(int id) {
	return getSeq(id, Friend);
}
MyUtil::IntSeq BuddyCoreSnapshotHelper::getApplySeq(int id) {
	return getSeq(id, Apply);
}
MyUtil::IntSeq BuddyCoreSnapshotHelper::getBlockSeq(int id) {
	return getSeq(id, Block);
}

MyUtil::IntSeq BuddyCoreSnapshotHelper::getSeq(int id, BuddyDesc desc) {
	SnapshotEntrySeqMap::iterator itOwner = _data.find(id);
	IntSeq result;
	if (itOwner == _data.end()) {
		MCE_WARN("CommonFriendSnapshot::getSeq [NOT FOUND] id=" << id << " desc=" << desc);
		return result;
	}
	for (SnapshotEntrySeq::iterator itFriend = itOwner->second.begin(); itFriend != itOwner->second.end(); itFriend++) {
		if (itFriend->desc == desc) {
			result.push_back(itFriend->toId);
		}
	}
	return result;
}

MyUtil::Int2IntSeqMap BuddyCoreSnapshotHelper::getBatch(const MyUtil::IntSeq& users,
		com::xiaonei::service::buddy::BuddyDesc desc) {
	Int2IntSeqMap result;
	for (IntSeq::const_iterator it = users.begin(); it!=users.end(); ++it) {
		IntSeq buddy = getSeq(*it, desc);
		if (buddy.empty()) {
			continue;
		}
		result.insert(make_pair(*it, buddy));
	}
	return result;
}


