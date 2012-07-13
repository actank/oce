#ifndef READFRIENDLISTFROMDISK_H_
#define READFRIENDLISTFROMDISK_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <Ice/Ice.h>
#include <IceUtil/Monitor.h>
#include <IceUtil/IceUtil.h>

#include "DbCxxPool/src/ConnectionPoolManager.h"
#include "DbCxxPool/src/QueryRunner.h"

#include "socialgraph/socialgraphutil/calculator/component.h"

namespace xce {
namespace socialgraph {

const static int PRODUCE_BATCH_SIZE = 5000;
const static int INDEX_SIZE = 10;

//-------------------------------------------------------------------

struct FriendListHead {
	int uid;
	int len;
};

//-------------------------------------------------------------------

class ReadFriendListFromDisk : public Component {
public:
	ReadFriendListFromDisk(const std::string path); 
	~ReadFriendListFromDisk();

	void LoadData();
	void GetFriendList(int userid, std::vector<int>& friendlist);
	bool LoadFriendListIndex();

	void Load() {
		LoadData();
	}

private:
	class PreloadThread: virtual public IceUtil::Thread {
	public:
		PreloadThread(ReadFriendListFromDisk* rf, int mod): 
			rf_(rf), mod_(mod) {
		}
		virtual void run(); 

	private:
		ReadFriendListFromDisk* rf_;
		int mod_;
	};

	void LoadFriendList(int mod, const std::map<int, std::vector<int> >& friendlist);
	void DecreaseRemain();
	int Tobuff(int id, const std::vector<int>& friendlist, char* buff) const;
	void Frombuff(int len, char* buff, std::vector<int>& friendlist);

	std::vector<std::string> pathlist_;
	std::vector<FILE*> fplist_;
	std::vector< __gnu_cxx::hash_map<int, int> > id2offset_;
	IceUtil::Monitor<IceUtil::Mutex> mutex_;
	int remain_;
	char* buff_;
};

//-------------------------------------------------------------------
//buddyrelation表名
class TableName {
public:
  static std::string RelationTableName(int id) {
    std::ostringstream oss;
    oss << "relation_" << id % 100;
    return oss.str();
  }

  static std::string RelationPattern(int id) {
    std::ostringstream oss;
    oss << "relation_" << id % 100;
    return oss.str();
  }
};

//-------------------------------------------------------------------
//批量获取好友列表
class BatchFriendListHandler : public com::xiaonei::xce::ResultHandler {
public:
	BatchFriendListHandler(int& last, std::map<int, std::vector<int> >& friendlist):
		last_(last), friendlist_(friendlist) {}
	virtual bool handle(mysqlpp::Row& row) const; 

private:
	int& last_;
	std::map<int, std::vector<int> >& friendlist_;
};

};
};

#endif
