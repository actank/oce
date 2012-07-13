#ifndef _HOTFEED_UPDATE_DBHELPER_
#define _HOTFEED_UPDATE_DBHELPER_

#include <QueryRunner.h>
#include "UGCItem.h" 

namespace xce {
namespace dbhelper {
static const int STATE_MULTI_PHOTO = 1 << 2;
static const int MULTI_PHOTO = 709;
static const int SINGLE_PHOTO = 701;
static const int BLOG = 601;

//--------------------------------------------------------------------------------------------

class HotFeedUpdateDBHelper {
public:
	static void GetUserPhotoUGC(int userId, std::vector<item::UGCItem>& ugcList, int begin, int end); 
	static void GetUserBlogUGC(int userId, std::vector<item::UGCItem>& ugcList, int begin, int end);
	static void GetUserBlockUGC(int accepterId, int userId, std::vector<long>& blockUgcList);
};

//--------------------------------------------------------------------------------------------

class PhotoDataReadHelper : public com::xiaonei::xce::ResultHandler {
public:
	PhotoDataReadHelper(std::vector<item::UGCItem>& ugcList): _ugcList(ugcList) {}
	virtual bool handle(mysqlpp::Row& row) const;
	bool IsMultiPhoto(int state) const;

private:
	std::vector<item::UGCItem>& _ugcList;
};

//--------------------------------------------------------------------------------------------

class BlogDataReadHelper : public com::xiaonei::xce::ResultHandler {
public:
	BlogDataReadHelper(std::vector<item::UGCItem>& ugcList): _ugcList(ugcList) {}
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::vector<item::UGCItem>& _ugcList;
};

//--------------------------------------------------------------------------------------------

class BlockUGCReadHelper : public com::xiaonei::xce::ResultHandler {
public:
	BlockUGCReadHelper(std::vector<long>& blockUgcList): _blockUgcList(blockUgcList) {}
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::vector<long>& _blockUgcList;
};

//--------------------------------------------------------------------------------------------

class TableName {
public:
	static std::string PhotoTableName(int id) {
		ostringstream oss;
		oss << "photo_" << (id % 100);
		return oss.str();
	}

	static std::string PhotoTablePattern(int id) {
		ostringstream oss;
		oss << "photo_" << (id % 100);
		return oss.str();
	}

	static std::string BlogTableName(int id) {
		ostringstream oss;
		oss << "blog_" << (id % 100);
		return oss.str();
	}

	static std::string BlogTablePattern(int id) {
		ostringstream oss;
		oss << "blog_" << (id % 100);
		return oss.str();
	};

	static std::string HotFeedBlockTable(int id) {
		ostringstream oss;
		oss << "hotfeedrecommend_block_" << (id % 10);  
		return oss.str();
	}
};

}
}

#endif
