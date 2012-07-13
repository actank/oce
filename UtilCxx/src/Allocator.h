#ifndef __HEAD_URL_UTIL_H__
#define __HEAD_URL_UTIL_H__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "IceLogger.h"
#include "IceExt/src/ServiceI.h"
#include "DbCxxPool/src/QueryRunner.h"

namespace com {
namespace xiaonei {
namespace platform {
namespace storage {

using namespace std;	

const string COLUMN_NODE_ID = "id";
const string COLUMN_NODE_NAME = "node_name";
const string COLUMN_GROUP_NAME = "group_name";
const string COLUMN_PATH = "path";
const string COLUMN_DOMAIN = "domain";
const string COLUMN_SPACE_USED = "space_used";
const string COLUMN_CPU_LOAD = "cpu_load";
const string COLUMN_TIME = "time";
const string COLUMN_STATUS = "status";
const string COLUMN_TYPE = "type";

const string STATE_SPACE = "Space";
const string STATE_LOAD = "Load";
const string STATE_TIME = "Time";
const int RANK_LEVEL_IDLE = 0;
const int RANK_LEVEL_NORMAL = 1;
const int RANK_LEVEL_WARN = 2;
const int RANK_LEVEL_FATAL = 3;
const int RANK_LEVEL_COUNT = 4;
const int TRAP_INTERVAL = 60*3;
const int NODE_ACTIVE_THRESHOLD = 60*8;
const double THRESHOLD_SPACEUSED = 0.90;
const double THRESHOLD_LOAD = 10;
const string TABLE_STORAGE_NODE = "storage_node";//node 的数据库表名
const int NODE_STATUS_DISABLE = 0;// node 状态无效
const int NODE_STATUS_WRITABLE = 2;//node 状态能写
const int NODE_STATUS_READABLE = 1;//node 状态能读
const int NODE_STATUS_WRITABLE_READABLE=3; //node 状态即读又能写
const int NODE_TYPE_NORMAL = 1;
const int NODE_TYPE_HEAD = 2;
const int NODE_TYPE_CLASS = 3 ;

const int LOAD_INTERVAL = 300;
const string FILE_SEPARATOR = "/";
const string URL_HEAD = "http://head.xiaonei.com";
const string URL_TINY_HEAD = "http://tiny.head.xiaonei.com";

// --------------------- Node ----------------------------------------
class Node : public Ice::Object {
public:
	Node():	spaceUsed(0.0) {
	}

	string getDomain() {
		return domain;
	}

	void setDomain(const string& domainName) {
		this->domain = domainName;
	}

	string getGroup() {
		return group;
	}

	void setGroup(const string& name) {
		group = name;
	}

	string getPath() {
		return path;
	}

	void setPath(const string& path) {
		this->path = path;
	}

	string getName() {
		return name;
	}

	void setName(const string& name) {
		this->name = name;
	}

	double getSpaceUsed() {
		return spaceUsed;
	}

	void setSpaceUsed(double space) {
		this->spaceUsed = space;
	}

	double getCpuLoad() {
		return cpuLoad;
	}

	void setCpuLoad(double load) {
		this->cpuLoad = load;
	}

	bool isReadable() {
		bool result = ((this->status & NODE_STATUS_WRITABLE) != 0)
			&& getSpaceUsed() < THRESHOLD_SPACEUSED;
		return result;
	}

	bool isWritable() {
		bool result = (this->status & NODE_STATUS_WRITABLE) != 0
		&& this->getSpaceUsed() < THRESHOLD_SPACEUSED;
		return result;
	}

	bool isReadableAndWritable() {
		return isWritable() && isWritable();
	}

	bool isTempReable() {
		bool result = (this->status == 4);
		return result;
	}

	int loadaverage() {
		double load = this->getCpuLoad();
		if (load <= 5) {
			return RANK_LEVEL_IDLE;
		} else if (load <= 8) {
			return RANK_LEVEL_NORMAL;
		} else if (load <= 12) {
			return RANK_LEVEL_WARN;
		} else { 
			return RANK_LEVEL_FATAL;
		}
	}

	int getId() {
		return id;
	}

	void setId(int id) {
		this->id = id;
	}

	long getTime() {
		return time;
	}

	void setTime(long time) {
		this->time = time;
	}

	int getStatus() {
		return status;
	}

	void setStatus(int status) {
		this->status = status;
	}

	int getType() {
		return type;
	}

	void setType(int type) {
		this->type = type;
	}

private: 
	int id;
	std::string name;
	std::string group;
	std::string path;
	double spaceUsed;
	double cpuLoad;
	int status;
	std::string domain;
	long time;
	int type;
};

typedef IceUtil::Handle<Node> NodePtr;


// -------------------- Group --------------------------------
class Group : virtual public Ice::Object {
	
public:	
	void addNode(const NodePtr& node) {
		nodes.push_back(node);
	}

	void getRandomOrder(vector<int>& order, int length) {
		order.clear();
		if ( length <= 0 ) {
			return;
		}
		for (int i = 0; i < length; ++i ) {
			order.push_back(i);
		}
		for (int i = length-1 ; i>=0; --i) 	{
			int j=rand()%(i+1);
			int t = order[j];
			order[j] = order[i];
			order[i] = t;
		}
	}

	NodePtr getReadableNode() {

		if ( nodes.size() <= 0 ) {
			return NULL;
		}

		vector<int> randomOrder;
		getRandomOrder(randomOrder, nodes.size());
		MCE_DEBUG("[Group::getReadableNode] random vec size : " << randomOrder.size());
		NodePtr node;
		for ( size_t i = 0; i < nodes.size(); ++i ) {
			node = nodes.at(randomOrder[i]);
			if ( node->isReadable() ) {
				return node;
			}
		}
		return nodes.at(randomOrder[0]);
	}

	vector<NodePtr> getNodes() {
		return nodes;
	}
	
	string getName() {
		return name;
	}

	void setName(const string& name) {
		this->name = name;
	}

	int getLoad() {
		double load = 0.0;
		for (size_t i = 0; i < nodes.size(); ++i) {
			NodePtr node = nodes.at(i);
			if (node->getCpuLoad() > load) {
				load = node->getCpuLoad();
			}
		}
		return (int)load;
	}
	
	int rank() {
		double load = getLoad();
		if (load <= 10) {
			return RANK_LEVEL_IDLE;
		} else if (load <= 30) {
			return RANK_LEVEL_NORMAL;
		} else if (load <= 50) {
			return RANK_LEVEL_WARN;
		} else {
			return RANK_LEVEL_FATAL;
		}
	}

	int getType() {
		if (nodes.empty()) {
			return -1;
		}
		return nodes.at(0)->getType();
	}

private:
	  string name;
	  vector<NodePtr> nodes;
};

typedef IceUtil::Handle<Group> GroupPtr;


// ----------------- Domain ----------------------------------
class Domain : virtual public Ice::Object,
		public IceUtil::Mutex {

public:
	Domain(const string& dbDesc):
		_dbDesc(dbDesc) {
		loadConfig();
	}

	GroupPtr getGroup(const string& groupName);
	void loadConfig();

	map<string, GroupPtr> getAllGroup(){
		return allGroups ;
	}

private:
	map<string, GroupPtr> allGroups;
	string _dbDesc;
};	

typedef IceUtil::Handle<Domain> DomainPtr;

// ----------------------- Allocator ------------------------------------
class Allocator : public MyUtil::Singleton<Allocator>,
			public IceUtil::Thread {

public:
	void loadConfig() { 
		domain->loadConfig();
	}

	string locate(const string& pattern);

	string locate(const string& pattern, int type);
	
	string getPhotoUrl(const string& pattern);

	GroupPtr locateGroup(const string& pattern);

	DomainPtr getDomain(){
		return domain ;
	}

	virtual void run();

private:
	friend class MyUtil::Singleton<Allocator>;
	Allocator() {
		domain = new Domain("upload");
		loadConfig();
		start();	// 开始循环load的线程
	};
	DomainPtr domain;
};

// ------------------ HeadUrlUtil ---------------------------------------
class HeadUrlUtil : public MyUtil::Singleton<HeadUrlUtil> {
public:
	/** 
	 * 得到用户头像的完整URL地址 
	 */
	string getHeadFullUrl(const string& pattern);

	/**
	 * 从外站得到用户头像的完整URL地址，避免图片防盗链
	 */
	string getHeadFullUrlFromOuterSite(const string& pattern);
private:
	friend class MyUtil::Singleton<HeadUrlUtil>;
	HeadUrlUtil() {
		Allocator::instance();
	}
};

// ================= ResultHandler部分 ======================================
// ---------------- GroupResultHandler ----------------------
class GroupResultHandler : public com::xiaonei::xce::ResultHandler {
public:
	GroupResultHandler(map<string, GroupPtr>& result):
		_result(result) {
	};
	virtual bool handle(mysqlpp::Row& row) const;
private:
	map<string, GroupPtr>& _result;
};

}
}
}
}

#endif
