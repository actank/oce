#include "socialgraph/homereconnect/writereconnect/update/relationbatchhelper.h"

namespace xce {
namespace relationhelper {
//从db中获取推荐列表
void RelationBatchHelper::getRecommendResult(int userId, MyUtil::Int2IntSeqMap& recommendResult, MyUtil::Int2IntMap& friendRank) {
	using namespace com::xiaonei::xce;
	std::string tableName = TableName::recommend_friends_table(userId);
	std::cout << "[RelationBatchHelper] getRecommendResult userId(" << userId << ") recommend_friends_talbe(" << tableName << ")" << std::endl;

	Statement oss;
	oss << "select owner_id, friend_id, friend_rank, strong_recommend_ids from "
		<< tableName << " where owner_id=" << userId;

	try {
		QueryRunner( "friendfinder_activeuser", CDbRServer, 
				tableName).query( oss, RecommendResultHandler(recommendResult, friendRank) );
	} catch ( Ice::Exception& e) {
		std::cout << "[RelationBatchHelper] getRecommendResult userId(" << userId << ") ice Exception " << e.what() << std::endl;
	} catch ( ... ) {
		std::cout << "[RelationBatchHelper] getRecommendResult userId(" << userId << ") unknow exception" << std::endl;
	}
}
//从db中获取block列表
void RelationBatchHelper::getBlockResult(int userId, MyUtil::Int2IntSeqMap& blockResult) {
	using namespace com::xiaonei::xce;
	std::string tableName = TableName::recommend_block_table(userId);
	//std::cout << "[RelationBatchHelper] getBlockResult userId(" << userId << ") recommend_block_table(" << tableName << ")" << std::endl;

	Statement oss;
	oss << "select owner_id, friend_id, block_ids from "
		<< tableName << " where owner_id=" << userId;

	try {
		QueryRunner( "friendfinder_activeuser", CDbRServer, 
				tableName).query( oss, BlockResultHandler(blockResult) );
	} catch ( Ice::Exception& e ) {
		std::cout << "[RelationBatchHelper] getBlockResult userId(" << userId <<") Ice Exception " << e.what() << std::endl;
	} catch ( ... ) {
		std::cout << "[RelationBatchHelper] getBlockResult userId(" << userId << ") unknow exception" << std::endl;
	}
}

//**********************************************************************************

RecommendResultHandler::RecommendResultHandler( MyUtil::Int2IntSeqMap& result, MyUtil::Int2IntMap& friendrank ):
	_recommendResult(result), _friendrank(friendrank) {
}

RecommendResultHandler::~RecommendResultHandler() {
}

bool RecommendResultHandler::handle( mysqlpp::Row& row ) const {
	_recommendResult[ ((int)row["friend_id"]) ] = MyUtil::IntSeqSerialize::unserialize( row["strong_recommend_ids"] );
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
	MyUtil::IntSeq blockSeq = MyUtil::IntSeqSerialize::unserialize( row["block_ids"] );
	_blockResult[ (int)row["friend_id"] ] = blockSeq;
	return true;
}

}
}
