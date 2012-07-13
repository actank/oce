#ifndef _PUKFEED_DBHELP_H_
#define _PUKFEED_DBHELP_H_

#include <QueryRunner.h>

namespace dbhelper {

class PUKFeedDBHelper {  
public:
    static void GetRemovePUKeed(int userId, int friendId, std::set<int>& blockSet);
};

//-----------------------------------------------------------------------------

class PUKFeedBlockHandler : public com::xiaonei::xce::ResultHandler {
public:
	PUKFeedBlockHandler(std::set<int>& blockSet) : _blockSet(blockSet) {}
	
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::set<int>& _blockSet;
};

//-----------------------------------------------------------------------------

class TableName {
public:
    static std::string PUKFeedBlockTable(int id) {
        ostringstream oss;        
        oss << "pukfeed_block_" << (id % 10);
        return oss.str();
    }                    
};

}

#endif
