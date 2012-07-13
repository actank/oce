#include "friendutils.h"
#include "ServiceI.h"
#include "BuddyByIdReplicaAdapter.h"

namespace xce {
namespace socialgraph {

void RelationBatchHelper::GetFriendSeqBatch(const MyUtil::IntSeq& users, MyUtil::Int2IntSeqMap& ff) {
	if (users.empty()) {
		return;
	}
	ff = xce::buddy::adapter::BuddyByIdCacheAdapter::instance().getFriendLists(users);
}

void RelationBatchHelper::GetFriendSeq(int id, MyUtil::IntSeq& friendlist) {
	MCE_DEBUG("RelationBatchHelper::getFriendSeq for " << id);
	friendlist = xce::buddy::adapter::BuddyByIdCacheAdapter::instance().getFriendListAsc(id, -1);
}

void RelationBatchHelper::GetApplySeq(int id, MyUtil::IntSeq& applylist) {
	using namespace ::com::xiaonei::xce;
	if (id<=0) {
		return;
	}

	string pattern = TableName::buddyapply_friendfinder_applicant_pattern(id);	
	Statement oss;
	oss << "SELECT accepter FROM " <<  TableName::buddyapply_friendfinder_applicant(id)
		<< " WHERE applicant = " << id << " LIMIT 100";
	RelationResultHandler rsh(applylist, "accepter");
	QueryRunner("buddyapply_friendfinder_applicant", CDbRServer, pattern).query(oss, rsh);

	MCE_DEBUG("[RelationResultHandler] getApplySeq id=" << id << " applyList size=" << applylist.size() <<
				" pattern(" << pattern << ")  tablename(" << TableName::buddyapply_friendfinder_applicant(id) << ")");
}

void RelationBatchHelper::GetBlockSeq(int id, MyUtil::IntSeq& blocklist) {
	using namespace ::com::xiaonei::xce;
	if (id<=0) {
		return;
	}
	Statement oss;
	oss << "SELECT guest FROM relation_block  WHERE host = " << id;
	RelationResultHandler rsh(blocklist, "guest");
	QueryRunner("friendfinder_block", CDbRServer).query(oss, rsh);
}

void RelationBatchHelper::GetCommonFriendsBlock(int id, MyUtil::IntSeq& commonblock) {
	using namespace ::com::xiaonei::xce;
	if (id<=0) {
		return;
	}
	Statement oss;
	oss << "SELECT block_id FROM "<<TableName::common_friends_block_table(id)<<" WHERE owner_id = " << id;
	RelationResultHandler rsh(commonblock, "block_id");
	QueryRunner("friendfinder_common_friends_new", CDbRServer, TableName::common_friends_block_pattern(id)).query(oss, rsh);
}

//--------------------------------------------------------------------

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

}
}
