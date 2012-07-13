#ifndef _RELATIONBATCH_HELPER_H_
#define _RELATIONBATCH_HELPER_H_

#include <string>
#include <QueryRunner.h>
#include <ObjectCacheI.h>
#include "IntSeqSerialize.h"

namespace xce {
namespace relationhelper{

class RelationBatchHelper {
public:
	static void getRecommendResult(int userId, MyUtil::Int2IntSeqMap& recommendResult, MyUtil::Int2IntMap& friendRank);	
	static void getBlockResult(int userId, MyUtil::Int2IntSeqMap& blockResult);	
};

//**********************************************************************************

class TableName {
public:
	static std::string recommend_friends_table(int id) {
		ostringstream oss;
		oss << "recommend_friends_" << (id % 10);	
		return oss.str();
	}
	
	static std::string recommend_block_table(int id) {
		return "recommend_friends_block";	
	}
};

//**********************************************************************************

class RecommendResultHandler : public com::xiaonei::xce::ResultHandler {
public:
	RecommendResultHandler( MyUtil::Int2IntSeqMap& result, MyUtil::Int2IntMap& friendrank );
	virtual ~RecommendResultHandler();

protected:
	virtual bool handle( mysqlpp::Row& row ) const;

private:
	MyUtil::Int2IntSeqMap& _recommendResult;
	MyUtil::Int2IntMap& _friendrank;
};

//**********************************************************************************

class BlockResultHandler : public com::xiaonei::xce::ResultHandler {
public:
	BlockResultHandler(MyUtil::Int2IntSeqMap& blockResult); 
	virtual ~BlockResultHandler();

protected:
	virtual bool handle( mysqlpp::Row& row ) const;

private: 
	MyUtil::Int2IntSeqMap& _blockResult;
};

}
}

#endif
