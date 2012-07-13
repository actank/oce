#include "RelationBatchHelper.h"

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::relationhelper;

void RelationBatchHelper::getRecommendResult(int userId, MyUtil::Int2IntSeqMap& recommendResult, MyUtil::Int2IntMap& friendRank) {
	string tableName = TableName::recommend_friends_table(userId);
	MCE_INFO("[RelationBatchHelper] getRecommendResult userId(" << userId << ") recommend_friends_talbe(" << tableName << ")");

	Statement oss;
	oss << "select owner_id, friend_id, friend_rank, strong_recommend_ids from "
		<< tableName << " where owner_id=" << userId;

	try {
		QueryRunner( "friendfinder_activeuser", CDbRServer, 
				tableName).query( oss, RecommendResultHandler(recommendResult, friendRank) );
	} catch ( Ice::Exception& e) {
		MCE_WARN( "[RelationBatchHelper] getRecommendResult userId(" << userId << ") ice Exception " << e.what() );
	} catch ( ... ) {
		MCE_WARN( "[RelationBatchHelper] getRecommendResult userId(" << userId << ") unknow exception" );
	}
}

void RelationBatchHelper::getBlockResult(int userId, MyUtil::Int2IntSeqMap& blockResult) {
	string tableName = TableName::recommend_block_table(userId);
	MCE_INFO("[RelationBatchHelper] getBlockResult userId(" << userId << ") recommend_block_table(" << tableName << ")");

	Statement oss;
	oss << "select owner_id, friend_id, block_ids from "
		<< tableName << " where owner_id=" << userId;

	try {
		QueryRunner( "friendfinder_activeuser", CDbRServer, 
				tableName).query( oss, BlockResultHandler(blockResult) );
	} catch ( Ice::Exception& e ) {
		MCE_WARN("[RelationBatchHelper] getBlockResult userId(" << userId <<") Ice Exception " << e.what());
	} catch ( ... ) {
		MCE_WARN("[RelationBatchHelper] getBlockResult userId(" << userId << ") unknow exception");
	}
}

//**********************************************************************************

RecommendResultHandler::RecommendResultHandler( MyUtil::Int2IntSeqMap& result, MyUtil::Int2IntMap& friendrank ):
	_recommendResult(result), _friendrank(friendrank) {
}

RecommendResultHandler::~RecommendResultHandler() {
}

bool RecommendResultHandler::handle( mysqlpp::Row& row ) const {
	_recommendResult[ ((int)row["friend_id"]) ] = IntSeqSerialize::unserialize( row["strong_recommend_ids"] );
	_friendrank[ ((int)row["friend_id"]) ] = (int)row["friend_rank"];
	return true;
}

//**********************************************************************************

BlockResultHandler::BlockResultHandler(MyUtil::Int2IntSeqMap& blockResult):
	 _blockResult(blockResult) {
}

BlockResultHandler::~BlockResultHandler() {
}

bool BlockResultHandler::handle( mysqlpp::Row& row ) const {
	IntSeq blockSeq = IntSeqSerialize::unserialize( row["block_ids"] );
	_blockResult[ (int)row["friend_id"] ] = blockSeq;
	return true;
}
